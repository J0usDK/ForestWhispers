namespace ItemEditor.Core.UndoRedo;

internal interface IUndoRedoCommand
{
    void Undo();
    void Redo();
    bool CanMerge(IUndoRedoCommand nextCommand);
    void Merge(IUndoRedoCommand nextCommand);
}
