using ItemEditor.Models.Contracts;
using ItemEditor.Models.Item;

namespace ItemEditor.Core.UndoRedo;

internal sealed class TraitCollectionCommand(IItemModel item, TraitInstance trait, bool isAdded) : IUndoRedoCommand
{
    public void Undo()
    {
        if (isAdded) item.RemoveTrait(trait);
        else item.AddTrait(trait);
    }

    public void Redo()
    {
        if (isAdded) item.AddTrait(trait);
        else item.RemoveTrait(trait);
    }

    public bool CanMerge(IUndoRedoCommand nextCommand) => false;
    public void Merge(IUndoRedoCommand nextCommand) { }
}
