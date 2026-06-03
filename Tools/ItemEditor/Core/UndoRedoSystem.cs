using ItemEditor.Models.Item;

namespace ItemEditor.Core;

internal interface IUndoCommand
{
    void Undo();
    void Redo();
    bool CanMerge(IUndoCommand nextCommand);
    void Merge(IUndoCommand nextCommand);
}

internal sealed class UndoRedoManager(int limit = 50)
{
    private readonly int _limit = limit;
    private readonly LinkedList<IUndoCommand> _undoList = [];
    private readonly LinkedList<IUndoCommand> _redoList = [];

    private bool _isExecuting;

    public bool CanUndo => _undoList.Count > 0 && !_isExecuting;
    public bool CanRedo => _redoList.Count > 0 && !_isExecuting;

    public void Push(IUndoCommand command)
    {
        if (_isExecuting) return;

        if (_undoList.Count > 0 && _undoList.Last!.Value.CanMerge(command))
        {
            _undoList.Last.Value.Merge(command);
            return;
        }

        _undoList.AddLast(command);
        if (_undoList.Count > _limit) _undoList.RemoveFirst();
        _redoList.Clear();
    }

    public void Undo()
    {
        if (!CanUndo) return;

        var cmd = _undoList.Last!.Value;
        _undoList.RemoveLast();

        _isExecuting = true;
        cmd.Undo();
        _isExecuting = false;

        _redoList.AddLast(cmd);
    }

    public void Redo()
    {
        if (!CanRedo) return;

        var cmd = _redoList.Last!.Value;
        _redoList.RemoveLast();

        _isExecuting = true;
        cmd.Redo();
        _isExecuting = false;

        _undoList.AddLast(cmd);
    }

    public void Clear()
    {
        _undoList.Clear();
        _redoList.Clear();
    }
}

internal sealed class FieldValueChangeCommand(TraitFieldValue field, object? oldValue, object? newValue) : IUndoCommand
{
    private readonly TraitFieldValue _field = field;
    private readonly object? _oldValue = oldValue;
    private object? _newValue = newValue;

    public void Undo() => _field.Value = _oldValue;
    public void Redo() => _field.Value = _newValue;

    public bool CanMerge(IUndoCommand nextCommand) => nextCommand is FieldValueChangeCommand nextCmd && nextCmd._field == _field;
    public void Merge(IUndoCommand nextCommand) => _newValue = ((FieldValueChangeCommand)nextCommand)._newValue;
}

internal sealed class TraitCollectionCommand(ItemModel item, TraitInstance trait, bool isAdded) : IUndoCommand
{
    public void Undo()
    {
        if (isAdded) item.Traits.Remove(trait);
        else item.Traits.Add(trait);
    }

    public void Redo()
    {
        if (isAdded) item.Traits.Add(trait);
        else item.Traits.Remove(trait);
    }

    public bool CanMerge(IUndoCommand nextCommand) => false;
    public void Merge(IUndoCommand nextCommand) { }
}
