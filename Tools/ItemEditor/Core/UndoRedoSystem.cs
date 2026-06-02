using ItemEditor.Models.Item;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ItemEditor.Core
{
    internal interface IUndoCommand
    {
        void Undo();
        void Redo();
        bool CanMerge(IUndoCommand nextCommand);
        void Merge(IUndoCommand nextCommand);
    }

    internal class UndoRedoManager
    {
        private readonly int _limit;
        private readonly LinkedList<IUndoCommand> _undoList = new();
        private readonly LinkedList<IUndoCommand> _redoList = new();

        private bool _isExecuting;

        public bool CanUndo => _undoList.Count > 0 && !_isExecuting;
        public bool CanRedo => _redoList.Count > 0 && !_isExecuting;

        public UndoRedoManager(int limit = 50)
        {
            _limit = limit;
        }

        public void Push(IUndoCommand command)
        {
            if (_isExecuting)
                return;

            if (_undoList.Count > 0 && _undoList.Last!.Value.CanMerge(command))
            {
                _undoList.Last.Value.Merge(command);
                return;
            }

            _undoList.AddLast(command);
            if (_undoList.Count > _limit)
                _undoList.RemoveFirst();
            _redoList.Clear();
        }

        public void Undo()
        {
            if (!CanUndo)
                return;

            var cmd = _undoList.Last!.Value;
            _undoList.RemoveLast();

            _isExecuting = true;
            cmd.Undo();
            _isExecuting = false;

            _redoList.AddLast(cmd);
        }

        public void Redo()
        {
            if (!CanRedo)
                return;

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

    internal class FieldValueChangeCommand : IUndoCommand
    {
        private readonly TraitFieldValue _field;
        private readonly object _oldValue;
        private object _newValue;

        public FieldValueChangeCommand(TraitFieldValue field, object oldValue, object newValue)
        {
            _field = field;
            _oldValue = oldValue;
            _newValue = newValue;
        }

        public void Undo() => _field.Value = _oldValue;
        public void Redo() => _field.Value = _newValue;

        public bool CanMerge(IUndoCommand nextCommand)
        {
            if (nextCommand is FieldValueChangeCommand nextCmd)
                return nextCmd._field == this._field;
            return false;
        }

        public void Merge(IUndoCommand nextCommand)
        {
            _newValue = ((FieldValueChangeCommand)nextCommand)._newValue;
        }
    }

    internal class TraitCollectionCommand : IUndoCommand
    {
        private readonly ItemModel _item;
        private readonly TraitInstance _trait;
        private readonly bool _isAdded;

        public TraitCollectionCommand(ItemModel item, TraitInstance trait, bool isAdded)
        {
            _item = item;
            _trait = trait;
            _isAdded = isAdded;
        }

        public void Undo()
        {
            if (_isAdded)
                _item.Traits.Remove(_trait);
            else
                _item.Traits.Add(_trait);
        }

        public void Redo()
        {
            if (_isAdded)
                _item.Traits.Add(_trait);
            else
                _item.Traits.Remove(_trait);
        }

        public bool CanMerge(IUndoCommand nextCommand) => false;
        public void Merge(IUndoCommand nextCommand) { }
    }
}
