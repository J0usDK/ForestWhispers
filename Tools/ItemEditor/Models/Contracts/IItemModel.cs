using ItemEditor.Models.Item;

namespace ItemEditor.Models.Contracts;

internal interface IItemModel : IItemData, ITrackableItem, IRevertibleItem
{
    new string ItemID { get; set; }
    new string Description { get; set; }
    new string GeometryPath { get; set; }
    new string IconPath { get; set; }
    new bool IsIDDuplicate { get; set; }

    event EventHandler<(string OldID, string NewID)>? ItemIDChanged;

    void AddTrait(TraitInstance trait);
    void RemoveTrait(TraitInstance trait);

    IItemModel Clone(string newID);
}
