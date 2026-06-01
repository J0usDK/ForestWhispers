using System.Collections.ObjectModel;
using ItemEditor.Core;

namespace ItemEditor.Models.Item
{
    internal class ItemModel : ViewModelBase
    {
        private string _itemID = string.Empty;
        public string ItemID
        {
            get => _itemID;
            set
            {
                _itemID = value;
                OnPropertyChanged();
            }
        }

        public ObservableCollection<TraitInstance> Traits { get; } = new();
    }
}
