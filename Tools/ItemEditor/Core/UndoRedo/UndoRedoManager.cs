namespace ItemEditor.Core.UndoRedo;

internal sealed class UndoRedoManager(int limit = 50)
{
    private readonly int _limit = limit;

    private readonly List<IUndoRedoCommand> _redoStack = new(limit);
    private readonly IUndoRedoCommand[] _undoBuffer = new IUndoRedoCommand[limit];
    private int _undoStart = 0;
    private int _undoCount = 0;

    private bool _isExecuting;

    public long CurrentVersion { get; private set; }
    private long _savedVersion = -1;

    public bool IsDirty => CurrentVersion != _savedVersion;
    public event Action? StateChanged;

    public bool CanUndo => _undoCount > 0 && !_isExecuting;
    public bool CanRedo => _redoStack.Count > 0 && !_isExecuting;

    public void MarkAsSaved()
    {
        _savedVersion = CurrentVersion;
        StateChanged?.Invoke();
    }

    public void Push(IUndoRedoCommand command)
    {
        if (_isExecuting) return;

        if (_undoCount > 0)
        {
            int lastIndex = (_undoStart + _undoCount - 1) % _limit;
            if (_undoBuffer[lastIndex].CanMerge(command))
            {
                _undoBuffer[lastIndex].Merge(command);
                return;
            }
        }

        int newIndex = (_undoStart + _undoCount) % _limit;
        _undoBuffer[newIndex] = command;

        if (_undoCount < _limit)
            _undoCount++;
        else
            _undoStart = (_undoStart + 1) % _limit;
        _redoStack.Clear();

        CurrentVersion++;
        StateChanged?.Invoke();
    }

    public void Undo()
    {
        if (!CanUndo) return;

        var lastIndex = (_undoStart + _undoCount - 1) % _limit;
        var cmd = _undoBuffer[lastIndex];

        _undoCount--;

        _isExecuting = true;
        cmd.Undo();
        _isExecuting = false;

        _redoStack.Add(cmd);

        CurrentVersion--;
        StateChanged?.Invoke();
    }

    public void Redo()
    {
        if (!CanRedo) return;

        int lastIndex = _redoStack.Count - 1;
        var cmd = _redoStack[lastIndex];
        _redoStack.RemoveAt(lastIndex);

        _isExecuting = true;
        cmd.Redo();
        _isExecuting = false;

        int newIndex = (_undoStart + _undoCount) % _limit;
        _undoBuffer[newIndex] = cmd;
        if (_undoCount < _limit) _undoCount++;
        else _undoCount = (_undoStart + 1) % _limit;

        CurrentVersion++;
        StateChanged?.Invoke();
    }

    public void Clear()
    {
        _undoStart = 0;
        _undoCount = 0;
        _redoStack.Clear();
        CurrentVersion = 0;
        _savedVersion = 0;
        Array.Clear(_undoBuffer);
    }
}
