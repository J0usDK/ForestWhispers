namespace ItemEditor.Models.Contracts;

internal interface IRevertibleItem
{
    bool CanUndo();
    void Undo();
    bool CanRedo();
    void Redo();
    void ClearHistory();
}
