using System.Collections.ObjectModel;
using System.ComponentModel;
using ItemEditor.Models.Item;

namespace ItemEditor.Models.Contracts;

internal interface IItemData : INotifyPropertyChanged
{
    string ItemID { get; }
    string? OriginalItemID { get; }
    ReadOnlyObservableCollection<TraitInstance> Traits { get; }
}
