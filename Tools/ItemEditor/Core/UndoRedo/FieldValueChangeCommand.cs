using ItemEditor.Models.Item;

namespace ItemEditor.Core.UndoRedo;

internal sealed class FieldValueChangeCommand(TraitFieldValue field, object? oldValue, object? newValue) : IUndoRedoCommand
{
    private readonly TraitFieldValue _field = field;
    private readonly object? _oldValue = oldValue;
    private object? _newValue = newValue;

    public void Undo() => _field.Value = _oldValue;
    public void Redo() => _field.Value = _newValue;

    public bool CanMerge(IUndoRedoCommand nextCommand) => nextCommand is FieldValueChangeCommand nextCmd && nextCmd._field == _field;
    public void Merge(IUndoRedoCommand nextCommand) => _newValue = ((FieldValueChangeCommand)nextCommand)._newValue;
}
