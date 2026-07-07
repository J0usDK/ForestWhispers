using System.Diagnostics;
using System.IO;
using ItemEditor.Models.Schema;
using ItemEditor.Services.Contracts;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace ItemEditor.ViewModels;

internal sealed partial class MainViewModel : ObservableObject
{
    private readonly ISchemaService _schemaService;
    private readonly IItemService _itemService;
    private readonly ISettingsService _settingsService;
    private readonly IDialogService _dialogService;
    private readonly IItemIDRegistryService _registryService;
    public IItemIDRegistryService RegistryService => _registryService;
    public ItemsListViewModel ItemsList { get; }
    public ItemEditorViewModel ItemEditor { get; }
    public TraitsListViewModel TraitsList { get; }

    private ItemTraitsSchema? _currentSchema;

    [ObservableProperty]
    [NotifyCanExecuteChangedFor(nameof(SaveAllCommand))]
    private bool _isBusy;

    public MainViewModel(ISchemaService schemaService, IItemService itemService, ISettingsService settingsService, IDialogService dialogService, IItemIDRegistryService registryService,
        ItemsListViewModel itemsListViewModel, ItemEditorViewModel itemEditorViewModel, TraitsListViewModel traitsListViewModel)
    {
        _schemaService = schemaService;
        _itemService = itemService;
        _settingsService = settingsService;
        _dialogService = dialogService;
        _registryService = registryService;

        ItemsList = itemsListViewModel;
        ItemEditor = itemEditorViewModel;
        TraitsList = traitsListViewModel;

        ItemsList.SelectedItemChanged += (sender, item) => ItemEditor.SetItem(item);
        TraitsList.RequestAddTrait += (sender, trait) => ItemEditor.AddTrait(trait);
        TraitsList.CanAddTraitPredicate = trait =>
        {
            if (ItemEditor.CurrentItem == null) return false;
            for (int i = 0; i < ItemEditor.CurrentItem.Traits.Count; i++)
            {
                if (string.Equals(ItemEditor.CurrentItem.Traits[i].Id, trait.Id, StringComparison.OrdinalIgnoreCase))
                    return false;
            }
            return true;
        };
        ItemEditor.TraitsChanged += (sender, args) => TraitsList.RefreshCommandStates();

        _registryService.PropertyChanged += (sender, args) =>
        {
            if (args.PropertyName == nameof(IItemIDRegistryService.HasAnyConflicts))
                SaveAllCommand.NotifyCanExecuteChanged();
        };

        _ = LoadWorkspaceFromSettingsAsync();
    }

    private async Task LoadWorkspaceFromSettingsAsync()
    {
        var settings = _settingsService.LoadSettings();
        if (!string.IsNullOrEmpty(settings.LastSchemaPath) && File.Exists(settings.LastSchemaPath))
            await LoadSchemaAsync(settings.LastSchemaPath);

        if (!string.IsNullOrEmpty(settings.LastItemsDirectory) && Directory.Exists(settings.LastItemsDirectory) && _currentSchema != null)
            await LoadItemsFromDirectoryAsync(settings.LastItemsDirectory);
    }

    [RelayCommand]
    private async Task SelectSchemaAsync()
    {
        var schemaPath = _dialogService.ShowOpenFileDialog("Select Schema JSON File", "JSON Files (*.json)|*.json|All Files (*.*)|*.*");
        if (!string.IsNullOrEmpty(schemaPath))
        {
            var currentSettings = _settingsService.LoadSettings();
            var newSettings = currentSettings with { LastSchemaPath = schemaPath };
            _settingsService.SaveSettings(newSettings);

            await LoadSchemaAsync(schemaPath);

            if (!string.IsNullOrEmpty(newSettings.LastItemsDirectory) && Directory.Exists(newSettings.LastItemsDirectory))
                await LoadItemsFromDirectoryAsync(newSettings.LastItemsDirectory);
        }
    }

    [RelayCommand]
    private void SelectProjectDirectory()
    {
        string? selectedPath = _dialogService.ShowOpenFolderDialog("Select CryEngine Project Assets Folder");

        if (!string.IsNullOrEmpty(selectedPath))
        {
            var currentSettings = _settingsService.LoadSettings();
            var newSettings = currentSettings with { LastProjectDirectory = selectedPath };
            _settingsService.SaveSettings(newSettings);
        }
    }

    [RelayCommand]
    private async Task SelectItemsDirectoryAsync()
    {
        var folderPath = _dialogService.ShowOpenFolderDialog("Select folder with Items (.json)");
        if (string.IsNullOrEmpty(folderPath)) return;

        var currentSettings = _settingsService.LoadSettings();
        var newSettings = currentSettings with { LastItemsDirectory = folderPath };
        _settingsService.SaveSettings(newSettings);

        await LoadItemsFromDirectoryAsync(folderPath);
    }

    [RelayCommand(CanExecute = nameof(CanExecuteSaveAll))]
    private async Task SaveAllAsync()
    {
        var activeSettings = _settingsService.LoadSettings();
        if (string.IsNullOrEmpty(activeSettings.LastItemsDirectory))
        {
            var folderPath = _dialogService.ShowOpenFolderDialog("Select destination folder to save Items");
            if (string.IsNullOrEmpty(folderPath)) return;
            activeSettings = activeSettings with { LastItemsDirectory = folderPath };
            _settingsService.SaveSettings(activeSettings);
        }

        IsBusy = true;
        try
        {
            var itemsToSave = ItemsList.GetAllItems().Where(item => item.IsDirty).ToList();
            if (itemsToSave.Count == 0) return;

            var saveTasks = itemsToSave.Select(async item =>
            {
                string filePath = Path.Combine(activeSettings.LastItemsDirectory, $"{item.ItemID}.json");
                await _itemService.SaveItemAsync(item, filePath);
                item.AcceptChanges();
            });

            await Task.WhenAll(saveTasks);
            Debug.WriteLine($"[SUCCESS] {itemsToSave.Count} items saved successfully.");
        }
        catch (Exception ex)
        {
            _dialogService.ShowError($"Failed to save items: {ex.Message}", "Save Error");
        }
        finally
        {
            IsBusy = false;
        }
    }

    private bool CanExecuteSaveAll() =>
        !IsBusy && !_registryService.HasAnyConflicts && !ItemsList.HasValidationErrors();

    private async Task LoadSchemaAsync(string path)
    {
        IsBusy = true;
        try
        {
            _currentSchema = await _schemaService.LoadSchemaAsync(path);
            TraitsList.Initialize(_currentSchema.Traits);
            ItemEditor.Initialize(_currentSchema.ItemTypes);
        }
        catch (Exception ex)
        {
            _dialogService.ShowError($"[ERROR]: Schema load failed:\n{ex.Message}", "Error");
        }
        finally
        {
            IsBusy = false;
        }
    }

    private async Task LoadItemsFromDirectoryAsync(string directoryPath)
    {
        if (_currentSchema == null) return;

        IsBusy = true;
        try
        {
            var items = await _itemService.LoadAllItemsParallelAsync(directoryPath, _currentSchema);
            ItemsList.Initialize(items);
        }
        catch (Exception ex)
        {
            _dialogService.ShowError($"Failed to load items:\n{ex.Message}", "Error");
        }
        finally
        {
            IsBusy = false;
        }
    }

    public bool RequestApplicationClose()
    {
        if (ItemsList.HasUnsavedChanges())
        {
            return _dialogService.ShowConfirm(
                "You have unsaved changes!\nAre you sure you want to exit and lose all unsaved progress?",
                "Unsaved Changes");
        }
        return true;
    }
}
