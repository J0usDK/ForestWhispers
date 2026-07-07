using ItemEditor.Core.Validation.Context;
using ItemEditor.Models.Item;

namespace ItemEditor.Models.Contracts;

internal interface IItemModel : IItemData, ITrackableItem, IRevertibleItem
{
    new string ItemID { get; set; }
    new bool IsIDDuplicate { get; set; }

    event EventHandler<(string OldID, string NewID)>? ItemIDChanged;

    new int ItemType { get; set; }

    void AddTrait(TraitInstance trait);
    void RemoveTrait(TraitInstance trait);

    IItemModel Clone(string newID, IPathValidationContext? pathValidationContext = null);
}
