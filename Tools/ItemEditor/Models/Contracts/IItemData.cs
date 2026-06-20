using System.Collections.ObjectModel;
using System.ComponentModel;
using ItemEditor.Models.Item;

namespace ItemEditor.Models.Contracts;

internal interface IItemData : INotifyPropertyChanged
{
    string ItemID { get; }
    string? OriginalItemID { get; }

    string Description { get; }
    string GeometryPath { get; }
    string IconPath { get; }

    ReadOnlyObservableCollection<TraitInstance> Traits { get; }
}
