using ItemEditor.Models.Item;

namespace ItemEditor.Core.UndoRedo;

internal sealed class TraitCollectionCommand(ItemModel item, TraitInstance trait, bool isAdded) : IUndoRedoCommand
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

    public bool CanMerge(IUndoRedoCommand nextCommand) => false;
    public void Merge(IUndoRedoCommand nextCommand) { }
}
