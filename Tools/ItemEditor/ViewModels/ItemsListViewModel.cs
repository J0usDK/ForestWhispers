using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Windows.Data;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using ItemEditor.Models.Contracts;
using ItemEditor.Services.Contracts;

namespace ItemEditor.ViewModels;

internal sealed partial class ItemsListViewModel : ObservableObject
{
    private readonly IItemService _itemService;
    private readonly IDialogService _dialogService;
    private readonly ISettingsService _settingsService;
    private readonly IItemIDRegistryService _registryService;

    public ObservableCollection<IItemModel> Items { get; } = [];
    public ICollectionView ItemsView { get; }

    [ObservableProperty]
    [NotifyCanExecuteChangedFor(nameof(DuplicateItemCommand))]
    [NotifyCanExecuteChangedFor(nameof(DeleteItemCommand))]
    private IItemModel? _selectedItem;

    [ObservableProperty]
    private string _searchText = string.Empty;

    public event EventHandler<IItemModel?>? SelectedItemChanged;

    public ItemsListViewModel(IItemService itemService, IDialogService dialogService, ISettingsService settingsService, IItemIDRegistryService registryService)
    {
        _itemService = itemService;
        _dialogService = dialogService;
        _settingsService = settingsService;
        _registryService = registryService;

        ItemsView = CollectionViewSource.GetDefaultView(Items);
        ItemsView.Filter = FilterItems;

        _registryService.ItemIDCollisionStateChanged += OnItemIDCollisionStateChanged;
    }

    partial void OnSelectedItemChanged(IItemModel? value) => SelectedItemChanged?.Invoke(this, value);

    partial void OnSearchTextChanged(string value) => ItemsView.Refresh();

    public void Initialize(IEnumerable<IItemModel> loadedItems)
    {
        ClearItems();

        foreach (var item in loadedItems)
        {
            Items.Add(item);
            RegisterItem(item);
        }
    }

    public void ClearItems()
    {
        foreach (var item in Items) UnregisterItem(item);
        Items.Clear();
        SelectedItem = null;
    }

    public IEnumerable<IItemModel> GetAllItems() => Items;
    public bool HasUnsavedChanges() => Items.Any(i => i.IsDirty);
    public bool HasValidationErrors() => Items.Any(i => i.HasErrors);

    [RelayCommand]
    private void CreateItem()
    {
        var newItem = _itemService.CreateNewItem("new_item");
        Items.Add(newItem);
        RegisterItem(newItem);

        SelectedItem = newItem;
        ItemsView.MoveCurrentTo(newItem);
    }

    [RelayCommand(CanExecute = nameof(CanExecuteItemAction))]
    private void DuplicateItem(object? parameter)
    {
        if ((parameter as IItemModel ?? SelectedItem) is not { } sourceItem) return;

        var duplicateItem = sourceItem.Clone($"{sourceItem.ItemID}_copy");
        Items.Add(duplicateItem);
        RegisterItem(duplicateItem);
        SelectedItem = duplicateItem;
    }

    [RelayCommand(CanExecute = nameof(CanExecuteItemAction))]
    private void DeleteItem(object? parameter)
    {
        if ((parameter as IItemModel ?? SelectedItem) is not { } item) return;

        if (!_dialogService.ShowConfirm(
            $"Are you sure you want to delete '{item.ItemID}'?\nThis action cannot be undone and will delete the file from your drive.",
            "Delete Item")) return;

        UnregisterItem(item);
        Items.Remove(item);
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

    private bool CanExecuteItemAction(object? parameter) =>
        (parameter as IItemModel ?? SelectedItem) != null;

    private bool FilterItems(object obj)
    {
        if (string.IsNullOrWhiteSpace(SearchText)) return true;
        return obj is IItemModel item && item.ItemID.Contains(SearchText, StringComparison.OrdinalIgnoreCase);
    }

    private void RegisterItem(IItemModel item)
    {
        _registryService.Register(item);
        item.ItemIDChanged += OnModelItemIDChanged;
        item.IsIDDuplicate = _registryService.IsIDDuplicate(item.ItemID);
    }

    private void UnregisterItem(IItemModel item)
    {
        item.ItemIDChanged -= OnModelItemIDChanged;
        _registryService.Unregister(item);
    }

    private void OnModelItemIDChanged(object? sender, (string oldID, string newID) e)
    {
        if (sender is IItemModel item)
        {
            _registryService.UpdateItemID(item, e.oldID, e.newID);
            item.IsIDDuplicate = _registryService.IsIDDuplicate(e.newID);
        }
    }

    private void OnItemIDCollisionStateChanged(string itemID)
    {
        bool isDuplicate = _registryService.IsIDDuplicate(itemID);

        var affectedItems = _registryService.GetItemsByID(itemID);
        foreach (var itemData in affectedItems)
        {
            if (itemData is IItemModel model)
                model.IsIDDuplicate = isDuplicate;
        }
    }
}