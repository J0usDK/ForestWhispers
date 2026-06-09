using ItemEditor.Models.Item;

namespace ItemEditor.Core.UndoRedo;

internal sealed class ItemIDChangeCommand(ItemModel item, string OldID, string NewID) : IUndoRedoCommand
{
    private readonly ItemModel _item = item;
    private readonly string _oldID = OldID;
    private string _newID = NewID;

    public void Undo() => _item.ItemID = _oldID;
    public void Redo() => _item.ItemID = _newID;
    public bool CanMerge(IUndoRedoCommand other) => other is ItemIDChangeCommand cmd && cmd._item == _item;
    public void Merge(IUndoRedoCommand other) => _newID = ((ItemIDChangeCommand)other)._newID;
}
