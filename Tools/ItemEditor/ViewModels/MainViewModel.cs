using ItemEditor.Core;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Services;
using Microsoft.Win32;
using System.Collections.ObjectModel;
using System.Diagnostics;
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

        // Right Panel: traits list and search
        public ObservableCollection<TraitDefinition> AvaliableTraits { get; } = new();

        private string _searchQuery = string.Empty;
        public string SearchQuery
        {
            get => _searchQuery;
            set
            {
                _searchQuery = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(FilteredTraits));
            }
        }

        public IEnumerable<TraitDefinition> FilteredTraits
        {
            get
            {
                if (string.IsNullOrEmpty(SearchQuery))
                    return AvaliableTraits;

                var q = SearchQuery.ToLower();
                return AvaliableTraits.Where(t =>
                    t.DisplayName.ToLower().Contains(q) ||
                    t.Id.ToLower().Contains(q) ||
                    t.Fields.Any(f => f.Name.ToLower().Contains(q)));
            }
        }

        // Commands
        public ICommand CreateItemCommand { get; }
        public ICommand AddTraitCommand { get; }
        public ICommand RemoveTraitCommand { get; }

        public ICommand SelectSchemaCommand { get; }
        public ICommand SelectItemsDirectoryCommand { get; }
        public ICommand SaveAllCommand { get; }

        public MainViewModel(ISchemaService schemaService, IItemService itemService, SettingsService settingsService)
        {
            _schemaService = schemaService;
            _itemService = itemService;
            _settingsService = settingsService;

            CreateItemCommand = new RelayCommand(_ => CreateNewItem());
            AddTraitCommand = new RelayCommand(ExecuteAddTrait, CanExecuteAddTrait);
            RemoveTraitCommand = new RelayCommand(ExecuteRemoveTrait);

            SelectSchemaCommand = new RelayCommand(_ => ExecuteSelectSchema());
            SelectItemsDirectoryCommand = new RelayCommand(_ => ExecuteSelectItemsDirectory());
            SaveAllCommand = new RelayCommand(_ => ExecuteSaveAll());

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
            var settings = _settingsService.LoadSettings();
            if (string.IsNullOrEmpty(settings.LastItemsDirectory)) return;

            foreach (var item in LoadedItems)
            {
                string filePath = System.IO.Path.Combine(settings.LastItemsDirectory, $"{item.ItemID}.json");
                _itemService.SaveItem(item, filePath);
            }

            Debug.WriteLine("[SUCCESS] All items saved.");
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
    }
}
