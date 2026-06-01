using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Windows.Input;
using ItemEditor.Core;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Services;

namespace ItemEditor.ViewModels
{
    internal class MainViewModel : ViewModelBase
    {
        private readonly ISchemaService _schemaService;
        private readonly IItemService _itemService;

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

        public MainViewModel(ISchemaService schemaService, IItemService itemService)
        {
            _schemaService = schemaService;
            _itemService = itemService;

            CreateItemCommand = new RelayCommand(_ => CreateNewItem());
            AddTraitCommand = new RelayCommand(ExecuteAddTrait, CanExecuteAddTrait);
            RemoveTraitCommand = new RelayCommand(ExecuteRemoveTrait);

            LoadSchema();
        }

        private void LoadSchema()
        {
            try
            {
                string schemaPath = @"C:\Users\Admin\Documents\CRYENGINE Projects\Whispers Project\Data\Schemas\item_traits_schema.json";
                var schema = _schemaService.LoadSchema(schemaPath);

                AvaliableTraits.Clear();
                foreach (var trait in schema.Traits)
                    AvaliableTraits.Add(trait);
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"[ERROR] Schema load failed: {ex.Message}");
            }
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
