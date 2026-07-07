using System.Collections.ObjectModel;
using System.ComponentModel;
using ItemEditor.Models.Item;

namespace ItemEditor.Models.Contracts;

internal interface IItemData : INotifyPropertyChanged
{
    string ItemID { get; }
    string? OriginalItemID { get; }
    int ItemType { get; }

    MetadataFieldValue Description { get; }
    MetadataFieldValue GeometryPath { get; }
    MetadataFieldValue IconPath { get; }
    MetadataFieldValue Weight { get; }

    ReadOnlyObservableCollection<TraitInstance> Traits { get; }
}
