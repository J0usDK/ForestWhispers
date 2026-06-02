using ItemEditor.Core;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Services;
using Microsoft.Win32;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Data;
using System.Windows.Input;

namespace ItemEditor.ViewModels
{
    internal class MainViewModel : ViewModelBase
    {
        private readonly ISchemaService _schemaService;
        private readonly IItemService _itemService;
        private readonly SettingsService _settingsService;

        private ItemTraitsSchema? _currentSchema;

        // Left Panel: items list
        public ObservableCollection<ItemModel> LoadedItems { get; } = new();

        private ItemModel? _selectedItem;
        public ItemModel? SelectedItem
        {
            get => _selectedItem;
            set
            {
                _selectedItem = value;
                OnPropertyChanged();
            }
        }

        public ICollectionView ItemsView { get; }
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
                    ItemsView.Refresh();
                }
            }
        }

        // Right Panel: traits list and search
        public ObservableCollection<TraitDefinition> AvaliableTraits { get; } = new();

        public ICollectionView TraitsView { get; }
        private string _searchTraitText = string.Empty;
        public string SearchTraitText
        {
            get => _searchTraitText;
            set
            {
                _searchTraitText = value;
                OnPropertyChanged();
                TraitsView.Refresh();
            }
        }

        private bool FilterItems(object obj)
        {
            if (string.IsNullOrEmpty(SearchItemText))
                return true;
            if (obj is ItemModel item)
                return item.ItemID.Contains(SearchItemText, StringComparison.OrdinalIgnoreCase);
            return false;
        }

        private bool FilterTraits(object obj)
        {
            if (string.IsNullOrEmpty(SearchTraitText))
                return true;
            if (obj is TraitDefinition trait)
                return trait.DisplayName.Contains(SearchTraitText, StringComparison.OrdinalIgnoreCase) ||
                    trait.Id.Contains(SearchTraitText, StringComparison.OrdinalIgnoreCase) ||
                    trait.Fields.Any(f => f.Name.Contains(SearchTraitText, StringComparison.OrdinalIgnoreCase));
            return false;
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

        public MainViewModel(ISchemaService schemaService, IItemService itemService, SettingsService settingsService)
        {
            _schemaService = schemaService;
            _itemService = itemService;
            _settingsService = settingsService;

            ItemsView = CollectionViewSource.GetDefaultView(LoadedItems);
            ItemsView.Filter = FilterItems;

            TraitsView = CollectionViewSource.GetDefaultView(AvaliableTraits);
            TraitsView.Filter = FilterTraits;

            CreateItemCommand = new RelayCommand(_ => CreateNewItem());
            AddTraitCommand = new RelayCommand(ExecuteAddTrait, CanExecuteAddTrait);
            RemoveTraitCommand = new RelayCommand(ExecuteRemoveTrait);
            DeleteItemCommand = new RelayCommand(ExecuteDeleteItem);
            SaveItemCommand = new RelayCommand(ExecuteSaveItem);
            DuplicateItemCommand = new RelayCommand(ExecuteDuplicateItem);

            SelectSchemaCommand = new RelayCommand(_ => ExecuteSelectSchema());
            SelectItemsDirectoryCommand = new RelayCommand(_ => ExecuteSelectItemsDirectory());
            SaveAllCommand = new RelayCommand(_ => ExecuteSaveAll());

            UndoCommand = new RelayCommand(
                execute: _ =>
                {
                    SelectedItem?.History.Undo();
                },
                canExecute: _ =>
                {
                    return SelectedItem != null && SelectedItem.History.CanUndo;
                }
            );
            RedoCommand = new RelayCommand(
                execute: _ =>
                {
                    SelectedItem?.History.Redo();
                },
                canExecute: _ =>
                {
                    return SelectedItem != null && SelectedItem.History.CanRedo;
                }
            );

            LoadWorkspaceFromSettings();
        }

        private void LoadWorkspaceFromSettings()
        {
            var settings = _settingsService.LoadSettings();

            if (!string.IsNullOrEmpty(settings.LastSchemaPath))
                LoadSchema(settings.LastSchemaPath);
            if (!string.IsNullOrEmpty(settings.LastItemsDirectory) && _currentSchema != null)
                LoadItemsFromDirectory(settings.LastItemsDirectory);
        }

        private void ExecuteSelectSchema()
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

                LoadSchema(dialog.FileName);

                if (!string.IsNullOrEmpty(settings.LastItemsDirectory))
                    LoadItemsFromDirectory(settings.LastItemsDirectory);
            }
        }

        private void ExecuteSelectItemsDirectory()
        {
            var dialog = new OpenFolderDialog
            {
                Title = "Select folder with Items (.json)"
            };

            if (dialog.ShowDialog() == true)
            {
                var settings = _settingsService.LoadSettings();
                settings.LastItemsDirectory = dialog.FolderName;
                _settingsService.SaveSettings(settings);

                LoadItemsFromDirectory(dialog.FolderName);
            }
        }

        private void ExecuteSaveAll()
        {
            bool hasErrors = LoadedItems
                .SelectMany(item => item.Traits)
                .SelectMany(trait => trait.Fields)
                .Any(field => field.HasErrors);

            if (hasErrors)
            {
                ItemEditor.Views.CustomMessageBox.Show(
                    "Cannot save items. There are validation errors in one or more fields.\nPlease fix the highlighted red fields and try again.",
                    "Validation Error");
                return;
            }

            var settings = _settingsService.LoadSettings();

            if (string.IsNullOrEmpty(settings.LastItemsDirectory))
            {
                var dialog = new OpenFolderDialog
                { 
                    Title = "Select destination folder to save Items"
                };

                if (dialog.ShowDialog() == true)
                {
                    settings.LastItemsDirectory = dialog.FolderName;
                    _settingsService.SaveSettings(settings);
                }
                else
                {
                    return;
                }
            }

            foreach (var item in LoadedItems)
            {
                string filePath = System.IO.Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
                _itemService.SaveItem(item, filePath);
            }

            Debug.WriteLine("[SUCCESS] All items saved.");
        }

        private void ExecuteSaveItem(object? parameter)
        {
            var target = parameter ?? SelectedItem;
            if (target == null || target is not ItemModel item)
                return;

            var settings = _settingsService.LoadSettings();
            if (string.IsNullOrEmpty(settings.LastItemsDirectory))
                return;

            string filePath = System.IO.Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
            try
            {
                _itemService.SaveItem(item, filePath);
            }
            catch (Exception ex)
            {
                ItemEditor.Views.CustomMessageBox.Show($"Failed to save item: {ex.Message}", "Save Error");
            }
        }

        private void ExecuteDuplicateItem(object? parameter)
        {
            var sourceItem = (parameter as ItemModel) ?? SelectedItem;
            if (sourceItem == null) return;

            string newID = $"{sourceItem.ItemID}_copy";

            var duplicateItem = _itemService.CreateNewItem(newID);
            foreach (var sourceTrait in sourceItem.Traits)
            {
                var traitDefinition = AvaliableTraits.FirstOrDefault(t => t.Id == sourceTrait.Id);
                if (traitDefinition != null)
                {
                    var newTrait = _itemService.CreateTraitInstance(traitDefinition);
                    foreach (var sourceField in sourceTrait.Fields)
                    {
                        var newField = newTrait.Fields.FirstOrDefault(f => f.Name == sourceField.Name);
                        if (newField != null)
                            newField.Value = sourceField.Value;
                    }
                    duplicateItem.Traits.Add(newTrait);
                }
            }
            LoadedItems.Add(duplicateItem);
            SelectedItem = duplicateItem;
        }

        private void LoadSchema(string path)
        {
            try
            {
                _currentSchema = _schemaService.LoadSchema(path);
                AvaliableTraits.Clear();

                foreach (var trait in _currentSchema.Traits)
                    AvaliableTraits.Add(trait);
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"[ERROR] Schema load failed: {ex.Message}");
            }
        }

        private void LoadItemsFromDirectory(string directoryPath)
        {
            if (_currentSchema == null)
                return;

            LoadedItems.Clear();
            var items = _itemService.LoadAllItems(directoryPath, _currentSchema);
            foreach (var item in items)
                LoadedItems.Add(item);
        }

        private void CreateNewItem()
        {
            string newId = $"new_item_{LoadedItems.Count + 1}";
            var newItem = _itemService.CreateNewItem(newId);

            LoadedItems.Add(newItem);
            SelectedItem = newItem;
        }

        private bool CanExecuteAddTrait(object? parameter)
        {
            if (SelectedItem == null || parameter is not TraitDefinition traitDefinition)
                return false;
            return !SelectedItem.Traits.Any(t => t.Id == traitDefinition.Id);
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
            if (SelectedItem == null || parameter is not TraitInstance traitInstance)
                return;
            SelectedItem.Traits.Remove(traitInstance);
        }

        private void ExecuteDeleteItem(object? parameter)
        {
            if (parameter == null || parameter is not ItemModel item)
                return;

            bool confirm = ItemEditor.Views.CustomMessageBox.ShowConfirm(
                $"Are you sure you want to delete '{item.ItemID}'?\nThis action cannot be undone and will delete the file from your drive.",
                "Delete Item");

            if (!confirm)
                return;

            LoadedItems.Remove(item);
            if (SelectedItem == item)
                SelectedItem = null;

            var settings = _settingsService.LoadSettings();
            if (!string.IsNullOrEmpty(settings.LastItemsDirectory))
            {
                string filePath = System.IO.Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
                if (System.IO.File.Exists(filePath))
                {
                    System.IO.File.Delete(filePath);
                    System.Diagnostics.Debug.WriteLine($"[DELETE] Removed file: {filePath}");
                }
            }
        }
    }
}
