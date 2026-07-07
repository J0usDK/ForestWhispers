using ItemEditor.Models.Item;

namespace ItemEditor.Core.UndoRedo;

internal sealed class ItemTypeChangeCommand(ItemModel item, int oldValue, int newValue) : IUndoRedoCommand
{
    private readonly ItemModel _item = item;
    private readonly int _oldValue = oldValue;
    private readonly int _newValue = newValue;

    public void Undo() => _item.ItemType = _oldValue;
    public void Redo() => _item.ItemType = _newValue;

    public bool CanMerge(IUndoRedoCommand other) => false;
    public void Merge(IUndoRedoCommand other) { }
}
