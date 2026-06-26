using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using ItemEditor.Models.Contracts;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Services.Contracts;
using System.ComponentModel;
using System.IO;
using System.Windows.Input;

namespace ItemEditor.ViewModels;

internal sealed partial class ItemEditorViewModel : ObservableObject
{
    private readonly IItemService _itemService;
    private readonly ISettingsService _settingsService;
    private readonly IDialogService _dialogService;
    private readonly IItemIDRegistryService _registryService;
    private readonly IEnginePathService _enginePathService;

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(HasItem))]
    [NotifyPropertyChangedFor(nameof(IsCurrentItemIDDuplicate))]
    [NotifyPropertyChangedFor(nameof(CanSave))]
    [NotifyCanExecuteChangedFor(nameof(SaveItemCommand))]
    [NotifyCanExecuteChangedFor(nameof(UndoCommand))]
    [NotifyCanExecuteChangedFor(nameof(RedoCommand))]
    private IItemModel? _currentItem;

    public bool HasItem => CurrentItem != null;
    public bool IsCurrentItemIDDuplicate => CurrentItem != null && _registryService.IsIDDuplicate(CurrentItem.ItemID);
    public bool CanSave => CurrentItem != null && CurrentItem.IsDirty && !IsCurrentItemIDDuplicate && !CurrentItem.HasErrors;

    public event EventHandler? TraitsChanged;

    public ItemEditorViewModel(IItemService itemService, ISettingsService settingsService, IDialogService dialogService, IItemIDRegistryService registryService, IEnginePathService enginePathService)
    {
        _itemService = itemService;
        _settingsService = settingsService;
        _dialogService = dialogService;
        _registryService = registryService;
        _enginePathService = enginePathService;

        _registryService.ItemIDCollisionStateChanged += (changedID) =>
        {
            if (CurrentItem != null && string.Equals(CurrentItem.ItemID, changedID, StringComparison.OrdinalIgnoreCase))
                RefreshEditorState();
        };
    }

    partial void OnCurrentItemChanging(IItemModel? oldValue, IItemModel? newValue)
    {
        if (oldValue != null)
            oldValue.PropertyChanged -= OnItemPropertyChanged;
    }

    partial void OnCurrentItemChanged(IItemModel? oldValue, IItemModel? newValue)
    {
        if (newValue != null)
            newValue.PropertyChanged += OnItemPropertyChanged;
        TraitsChanged?.Invoke(this, EventArgs.Empty);
        RefreshEditorState();
    }

    public void SetItem(IItemModel? item) => CurrentItem = item;

    public void AddTrait(TraitDefinition traitDefinition)
    {
        if (CurrentItem == null || CurrentItem.Traits.Any(t => t.Id == traitDefinition.Id)) return;

        var newTraitInstance = _itemService.CreateTraitInstance(traitDefinition);
        CurrentItem.AddTrait(newTraitInstance);
        TraitsChanged?.Invoke(this, EventArgs.Empty);
        CommandManager.InvalidateRequerySuggested();
    }

    [RelayCommand]
    private void RemoveTrait(TraitInstance traitInstance)
    {
        if (CurrentItem == null) return;
        CurrentItem.RemoveTrait(traitInstance);
        TraitsChanged?.Invoke(this, EventArgs.Empty);
        RefreshEditorState();
    }

    [RelayCommand(CanExecute = nameof(CanSave))]
    private async Task SaveItemAsync()
    {
        if (CurrentItem == null) return;

        var settings = _settingsService.LoadSettings();
        if (string.IsNullOrWhiteSpace(settings.LastItemsDirectory))
        {
            _dialogService.ShowError("[ERROR] Items directory is not set. Cannot save item.", "Save Error");
            return;
        }

        string filePath = Path.Combine(settings.LastItemsDirectory, $"{CurrentItem.ItemID}.json");

        try
        {
            await _itemService.SaveItemAsync(CurrentItem, filePath);
            CurrentItem.AcceptChanges();
            RefreshEditorState();
        }
        catch (Exception ex)
        {
            _dialogService.ShowError($"[ERROR] Failed to save item: {ex.Message}", "Save Error");
        }
    }

    private bool CanUndo() => CurrentItem?.CanUndo() ?? false;
    [RelayCommand(CanExecute = nameof(CanUndo))]
    private void Undo() => CurrentItem?.Undo();

    private bool CanRedo() => CurrentItem?.CanRedo() ?? false;
    [RelayCommand(CanExecute = nameof(CanRedo))]
    private void Redo() => CurrentItem?.Redo();

    [RelayCommand]
    private void SelectGeometryPath()
    {
        if (CurrentItem == null) return;

        string? selectedPath = _dialogService.ShowOpenFileDialog(
            filter: "CryEngine Geometry (*.cgf)|*.cgf|All Files (*.*)|*.*",
            title: "Select Geometry File");

        if (!string.IsNullOrWhiteSpace(selectedPath))
            CurrentItem.GeometryPath.Value = _enginePathService.ConvertToEnginePath(selectedPath, _settingsService.LoadSettings().LastProjectDirectory);
    }

    [RelayCommand]
    private void SelectIconPath()
    {
        if (CurrentItem == null) return;

        string? selectedPath = _dialogService.ShowOpenFileDialog(
            filter: "CryEngine Surface (*.tif)|*.tif|All Files (*.*)|*.*",
            title: "Select Icon File");

        if (!string.IsNullOrWhiteSpace(selectedPath))
            CurrentItem.IconPath.Value = _enginePathService.ConvertToEnginePath(selectedPath, _settingsService.LoadSettings().LastProjectDirectory);
    }

    [RelayCommand]
    private void ConvertGeometryPath()
    {
        if (CurrentItem == null) return;
        TryConvertFieldToRelativePath(CurrentItem.GeometryPath);
    }

    [RelayCommand]
    private void ConvertIconPath()
    {
        if (CurrentItem == null) return;
        TryConvertFieldToRelativePath(CurrentItem.IconPath);
    }

    private void OnItemPropertyChanged(object? sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName is nameof(ITrackableItem.IsDirty) or nameof(ITrackableItem.HasErrors) or nameof(IItemData.ItemID))
            RefreshEditorState();
    }

    private void RefreshEditorState()
    {
        OnPropertyChanged(nameof(IsCurrentItemIDDuplicate));
        OnPropertyChanged(nameof(CanSave));

        SaveItemCommand.NotifyCanExecuteChanged();
        UndoCommand.NotifyCanExecuteChanged();
        RedoCommand.NotifyCanExecuteChanged();
    }

    private void TryConvertFieldToRelativePath(MetadataFieldValue field)
    {
        if (string.IsNullOrWhiteSpace(field.Value))
            return;

        string rootDir = _settingsService.LoadSettings().LastProjectDirectory;
        if (string.IsNullOrWhiteSpace(rootDir))
        {
            _dialogService.ShowError("The project root directory should be selected.");
            return;
        }

        string converted = _enginePathService.ConvertToEnginePath(field.Value, rootDir);
        if (!string.IsNullOrEmpty(converted) && converted != field.Value)
            field.Value = converted;
    }
}
