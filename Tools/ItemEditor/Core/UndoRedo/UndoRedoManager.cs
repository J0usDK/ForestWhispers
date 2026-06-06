namespace ItemEditor.Core.UndoRedo;

internal sealed class UndoRedoManager(int limit = 50)
{
    private readonly int _limit = limit;
    private readonly LinkedList<IUndoRedoCommand> _undoList = [];
    private readonly LinkedList<IUndoRedoCommand> _redoList = [];

    private bool _isExecuting;

    public bool CanUndo => _undoList.Count > 0 && !_isExecuting;
    public bool CanRedo => _redoList.Count > 0 && !_isExecuting;

    public void Push(IUndoRedoCommand command)
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
