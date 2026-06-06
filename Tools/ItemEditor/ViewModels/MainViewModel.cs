using Microsoft.Win32;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Windows.Data;
using System.Windows.Input;
using ItemEditor.Core;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Services;
using ItemEditor.Services.Contracts;
using ItemEditor.Core.Commands;

namespace ItemEditor.ViewModels;

internal sealed class MainViewModel : ViewModelBase
{
    private readonly ISchemaService _schemaService;
    private readonly IItemService _itemService;
    private readonly ISettingsService _settingsService;
    private readonly IDialogService _dialogService;
    private readonly ItemIDRegistryService _registryService;

    public ItemIDRegistryService RegistryService => _registryService;

    private ItemTraitsSchema? _currentSchema;

    private CancellationTokenSource? _searchItemDebounceToken;
    private CancellationTokenSource? _searchTraitDebounceToken;

    private bool _isBusy;
    public bool IsBusy
    {
        get => _isBusy;
        set
        {
            if (_isBusy != value)
            {
                _isBusy = value;
                OnPropertyChanged();
            }
        }
    }

    // Left Panel: items list
    private ObservableCollection<ItemModel> _loadedItems = [];
    public ObservableCollection<ItemModel> LoadedItems
    {
        get => _loadedItems;
        private set
        {
            _loadedItems = value;
            OnPropertyChanged();
        }
    }


    private ItemModel? _selectedItem;
    public ItemModel? SelectedItem
    {
        get => _selectedItem;
        set
        {
            if (_selectedItem != null) _selectedItem.PropertyChanged -= SelectedItem_PropertyChanged;
            _selectedItem = value;
            if (_selectedItem != null) _selectedItem.PropertyChanged += SelectedItem_PropertyChanged;

            OnPropertyChanged();
            OnPropertyChanged(nameof(IsSelectedItemIDDuplicate));
            OnPropertyChanged(nameof(CanSaveSelected));
        }
    }

    public bool IsSelectedItemIDDuplicate => _registryService.IsIDDuplicate(SelectedItem?.ItemID);
    public bool CanSaveSelected => SelectedItem != null && SelectedItem.IsDirty && !IsSelectedItemIDDuplicate && !SelectedItem.HasErrors;

    public ICollectionView ItemsView { get; private set; }

    private string _searchItemText = string.Empty;
    public string SearchItemText
    {
        get => _searchItemText;
        set
        {
            if (_searchItemText != value)
            {
                _searchItemText = value;
                OnPropertyChanged();
                DebounceSearchItems();
            }
        }
    }

    // Right Panel: traits list and search
    public ObservableCollection<TraitDefinition> AvaliableTraits { get; } = [];

    public ICollectionView TraitsView { get; }

    private string _searchTraitText = string.Empty;
    public string SearchTraitText
    {
        get => _searchTraitText;
        set
        {
            if (_searchTraitText != value)
            {
                _searchTraitText = value;
                OnPropertyChanged();
                DebounceSearchTraits();
            }
        }
    }

    // Commands
    public ICommand CreateItemCommand { get; }
    public ICommand AddTraitCommand { get; }
    public ICommand RemoveTraitCommand { get; }
    public ICommand DeleteItemCommand { get; }
    public ICommand SaveItemCommand { get; }
    public ICommand DuplicateItemCommand { get; }
    public ICommand SelectSchemaCommand { get; }
    public ICommand SelectItemsDirectoryCommand { get; }
    public ICommand SaveAllCommand { get; }
    public ICommand UndoCommand { get; }
    public ICommand RedoCommand { get; }

    public MainViewModel(ISchemaService schemaService, IItemService itemService, ISettingsService settingsService, IDialogService dialogService, ItemIDRegistryService registryService)
    {
        _schemaService = schemaService;
        _itemService = itemService;
        _settingsService = settingsService;
        _dialogService = dialogService;

        _registryService = registryService;
        _registryService.ConflictsChanged += () => CommandManager.InvalidateRequerySuggested();


        ItemsView = CollectionViewSource.GetDefaultView(LoadedItems);
        ItemsView.Filter = FilterItems;

        TraitsView = CollectionViewSource.GetDefaultView(AvaliableTraits);
        TraitsView.Filter = FilterTraits;

        CreateItemCommand = new RelayCommand(_ => CreateNewItem());
        AddTraitCommand = new RelayCommand(ExecuteAddTrait, CanExecuteAddTrait);
        RemoveTraitCommand = new RelayCommand(ExecuteRemoveTrait);
        DuplicateItemCommand = new RelayCommand(ExecuteDuplicateItem);

        SelectSchemaCommand = new RelayCommand(async _ => await ExecuteSelectSchemaAsync());
        SelectItemsDirectoryCommand = new RelayCommand(async _ => await ExecuteSelectItemsDirectoryAsync());
        SaveAllCommand = new RelayCommand(async _ => await ExecuteSaveAllAsync());
        SaveItemCommand = new RelayCommand(
            async parameter => await ExecuteSaveItemAsync(parameter),
            parameter =>
            {
                var item = parameter as ItemModel ?? SelectedItem;
                return item != null && item.IsDirty && !_registryService.IsIDDuplicate(item.ItemID) && !item.HasErrors;
            }
        );
        DeleteItemCommand = new RelayCommand(ExecuteDeleteItem);

        UndoCommand = new RelayCommand(_ => SelectedItem?.History.Undo(), _ => SelectedItem?.History.CanUndo ?? false);
        RedoCommand = new RelayCommand(_ => SelectedItem?.History.Redo(), _ => SelectedItem?.History.CanRedo ?? false);

        _ = LoadWorkspaceFromSettingsAsync();
    }

    private async void DebounceSearchItems()
    {
        _searchItemDebounceToken?.Cancel();
        _searchItemDebounceToken = new CancellationTokenSource();
        var token = _searchItemDebounceToken.Token;

        try
        {
            await Task.Delay(250, token);
            if (!token.IsCancellationRequested) ItemsView.Refresh();
        }
        catch(TaskCanceledException) { }
    }

    private async void DebounceSearchTraits()
    {
        _searchTraitDebounceToken?.Cancel();
        _searchTraitDebounceToken = new CancellationTokenSource();
        var token = _searchTraitDebounceToken.Token;

        try
        {
            await Task.Delay(250, token);
            if (!token.IsCancellationRequested) TraitsView.Refresh();
        }
        catch(TaskCanceledException) { }
    }

    private bool FilterItems(object obj)
    {
        if (string.IsNullOrEmpty(SearchItemText)) return true;
        return obj is ItemModel item && item.ItemID.Contains(SearchItemText, StringComparison.OrdinalIgnoreCase);
    }

    private bool FilterTraits(object obj)
    {
        if (string.IsNullOrEmpty(SearchTraitText)) return true;
        if (obj is not TraitDefinition trait) return false;

        return trait.DisplayName.Contains(SearchTraitText, StringComparison.OrdinalIgnoreCase)
            || trait.Id.Contains(SearchTraitText, StringComparison.OrdinalIgnoreCase)
            || trait.Fields.Any(f => f.Name.Contains(SearchTraitText, StringComparison.OrdinalIgnoreCase));
    }

    private async Task LoadWorkspaceFromSettingsAsync()
    {
        var settings = _settingsService.LoadSettings();

        if (!string.IsNullOrEmpty(settings.LastSchemaPath))
            await LoadSchemaAsync(settings.LastSchemaPath);
        if (!string.IsNullOrEmpty(settings.LastItemsDirectory) && _currentSchema != null)
            await LoadItemsFromDirectoryAsync(settings.LastItemsDirectory);
    }

    private async Task ExecuteSelectSchemaAsync()
    {
        var dialog = new OpenFileDialog
        {
            Title = "Select schema json file",
            Filter = "JSON Files (*.json)|*.json|All Files (*.*)|*.*"
        };

        if (dialog.ShowDialog() == true)
        {
            var settings = _settingsService.LoadSettings();
            settings.LastSchemaPath = dialog.FileName;
            _settingsService.SaveSettings(settings);

            await LoadSchemaAsync(dialog.FileName);

            if (!string.IsNullOrEmpty(settings.LastItemsDirectory))
                await LoadItemsFromDirectoryAsync(settings.LastItemsDirectory);
        }
    }

    private async Task ExecuteSelectItemsDirectoryAsync()
    {
        var folderPath = _dialogService.ShowOpenFolderDialog("Select folder with Items (.json)");

        if (!string.IsNullOrEmpty(folderPath))
        {
            var settings = _settingsService.LoadSettings();
            settings.LastItemsDirectory = folderPath;
            _settingsService.SaveSettings(settings);

            await LoadItemsFromDirectoryAsync(folderPath);
        }
    }

    private async Task ExecuteSaveAllAsync()
    {
        if (_registryService.TotalConflicts > 0 || LoadedItems.Any(item => item.HasErrors))
        {
            _dialogService.ShowError(
                "Cannot save items. There are validation errors in one or more fields.\nPlease fix the highlighted red fields and try again.",
                "Validation Error");
            return;
        }

        var settings = _settingsService.LoadSettings();

        if (string.IsNullOrEmpty(settings.LastItemsDirectory))
        {
            var folderPath = _dialogService.ShowOpenFolderDialog("Select destination folder to save Items");
            if (!string.IsNullOrEmpty(folderPath))
            {
                settings.LastItemsDirectory = folderPath;
                _settingsService.SaveSettings(settings);
            }
            else return;
        }

        IsBusy = true;
        try
        {
            var saveTasks = LoadedItems.Select(item =>
            {
                string filePath = Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
                return _itemService.SaveItemAsync(item, filePath);
            });

            await Task.WhenAll(saveTasks);
            Debug.WriteLine("[SUCCESS] All items saved.");
        }
        finally
        {
            IsBusy = false;
        }
    }

    private async Task ExecuteSaveItemAsync(object? parameter)
    {
        if ((parameter as ItemModel ?? SelectedItem) is not { } item) return;

        var settings = _settingsService.LoadSettings();
        if (string.IsNullOrEmpty(settings.LastItemsDirectory)) return;

        string filePath = Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
        IsBusy = true;
        try
        {
            await _itemService.SaveItemAsync(item, filePath);
        }
        catch (Exception ex)
        {
            _dialogService.ShowError($"Failed to save item: {ex.Message}", "Save Error");
        }
        finally
        {
            IsBusy = false;
        }
    }

    private async Task LoadSchemaAsync(string path)
    {
        IsBusy = true;
        try
        {
            _currentSchema = await _schemaService.LoadSchemaAsync(path);
            
            AvaliableTraits.Clear();
            foreach (var trait in _currentSchema.Traits)
                AvaliableTraits.Add(trait);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"[ERROR] Schema load failed: {ex.Message}");
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
            foreach (var item in items) RegisterModel(item);

            LoadedItems = new ObservableCollection<ItemModel>(items);
            ItemsView = CollectionViewSource.GetDefaultView(LoadedItems);
            ItemsView.Filter = FilterItems;

            OnPropertyChanged(nameof(ItemsView));
        }
        finally
        {
            IsBusy = false;
        }
    }

    private void CreateNewItem()
    {
        var newItem = _itemService.CreateNewItem($"new_item");

        RegisterModel(newItem);
        LoadedItems.Add(newItem);
        SelectedItem = newItem;
    }

    private void ExecuteDuplicateItem(object? parameter)
    {
        if ((parameter as ItemModel ?? SelectedItem) is not { } sourceItem) return;

        var duplicateItem = sourceItem.Clone($"{sourceItem.ItemID}_copy");

        RegisterModel(duplicateItem);
        LoadedItems.Add(duplicateItem);
        SelectedItem = duplicateItem;
    }

    private bool CanExecuteAddTrait(object? parameter)
    {
        return (SelectedItem != null && parameter is TraitDefinition traitDefinition
            && !SelectedItem.Traits.Any(t => t.Id == traitDefinition.Id));
    }

    private void ExecuteAddTrait(object? parameter)
    {
        if (SelectedItem == null || parameter is not TraitDefinition traitDefinition)
            return;

        var newTraitInstance = _itemService.CreateTraitInstance(traitDefinition);
        SelectedItem.Traits.Add(newTraitInstance);
        CommandManager.InvalidateRequerySuggested();
    }

    private void ExecuteRemoveTrait(object? parameter)
    {
        if (SelectedItem != null && parameter is TraitInstance traitInstance)
            SelectedItem.Traits.Remove(traitInstance);
    }

    private void ExecuteDeleteItem(object? parameter)
    {
        if (parameter is not ItemModel item) return;

        if (!_dialogService.ShowConfirm(
            $"Are you sure you want to delete '{item.ItemID}'?\nThis action cannot be undone and will delete the file from your drive.",
            "Delete Item")) return;

        UnregisterModel(item);
        LoadedItems.Remove(item);
        if (SelectedItem == item) SelectedItem = null;

        var settings = _settingsService.LoadSettings();
        if (!string.IsNullOrEmpty(settings.LastItemsDirectory))
        {
            string filePath = Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
            if (File.Exists(filePath))
            {
                File.Delete(filePath);
                Debug.WriteLine($"[DELETE] Removed file: {filePath}");
            }
        }
    }

    public bool RequestApplicationClose()
    {
        bool hasUnsavedChanges = LoadedItems.Any(item => item.IsDirty);
        if (hasUnsavedChanges)
        {
            return _dialogService.ShowConfirm(
                "You have unsaved changes!\nAre you sure you want to exit and lose all unsaved progress?",
                "Unsaved Changes");
        }
        return true;
    }

    private void RegisterModel(ItemModel item)
    {
        _registryService.Register(item);
        item.ItemIDChanged += OnModelItemIDChanged;
    }

    private void UnregisterModel(ItemModel item)
    {
        item.ItemIDChanged -= OnModelItemIDChanged;
        _registryService.Unregister(item);
    }

    private void OnModelItemIDChanged(object? sender, (string oldID, string newID) e)
    {
        if (sender is ItemModel item)
            _registryService.UpdateItemID(item, e.oldID, e.newID);

        if (sender == SelectedItem)
        {
            OnPropertyChanged(nameof(IsSelectedItemIDDuplicate));
            OnPropertyChanged(nameof(CanSaveSelected));
        }
    }

    private void SelectedItem_PropertyChanged(object? sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName is nameof(ItemModel.IsDirty) or nameof(ItemModel.HasErrors))
            OnPropertyChanged(nameof(CanSaveSelected));
    }
}
