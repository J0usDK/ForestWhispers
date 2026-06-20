using ItemEditor.Models.Item;

namespace ItemEditor.Core.UndoRedo;

internal sealed class ItemMetadataChangeCommand(ItemModel item,
    string propertyName, Action<string> setter,
    string oldValue, string newValue) : IUndoRedoCommand
{
    private readonly ItemModel _item = item;
    private readonly string _propertyName = propertyName;
    private readonly Action<string> _setter = setter;
    private readonly string _oldValue = oldValue;
    private string _newValue = newValue;

    public void Undo() => _setter(_oldValue);
    public void Redo() => _setter(_newValue);
    public bool CanMerge(IUndoRedoCommand other) => other is ItemMetadataChangeCommand cmd && cmd._item == _item && cmd._propertyName == _propertyName;
    public void Merge(IUndoRedoCommand other) => _newValue = ((ItemMetadataChangeCommand)other)._newValue;
}
