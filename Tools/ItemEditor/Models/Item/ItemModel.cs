using ItemEditor.Core;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace ItemEditor.Models.Item
{
    internal class ItemModel : ViewModelBase
    {
        public UndoRedoManager History { get; } = new UndoRedoManager(50);

        private string _itemID = string.Empty;
        public string ItemID
        {
            get => _itemID;
            set
            {
                if (_itemID != value)
                {
                    _itemID = value;
                    OnPropertyChanged();
                    OnPropertyChanged(nameof(IsDirty));
                }
            }
        }

        private string? _originalItemID;
        private List<string> _originalTraitIDs = new List<string>();
        public bool IsDirty
        {
            get
            {
                if (_originalItemID == null)
                    return true;

                if (ItemID != _originalItemID)
                    return true;

                if (Traits.Count != _originalTraitIDs.Count)
                    return true;

                for (int i = 0; i < Traits.Count; i++)
                {
                    if (Traits[i].Id != _originalTraitIDs[i])
                        return true;
                }

                return Traits.Any(t => t.Fields.Any(f => f.IsDirty));
            }
        }

        public ObservableCollection<TraitInstance> Traits { get; }
        public bool HasErrors => Traits.Any(t => t.Fields.Any(f => f.HasErrors));

        public ItemModel()
        {
            Traits = new ObservableCollection<TraitInstance>();
            Traits.CollectionChanged += Traits_CollectionChanged;
        }

        public void AcceptChanges()
        {
            _originalItemID = ItemID;
            _originalTraitIDs = Traits.Select(t => t.Id).ToList();

            foreach (var trait in Traits)
            {
                foreach (var field in trait.Fields)
                    field.AcceptChanges();
            }
            OnPropertyChanged(nameof(IsDirty));
        }

        private void Traits_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            OnPropertyChanged(nameof(IsDirty));
            OnPropertyChanged(nameof(HasErrors));

            if (e.NewItems != null)
            {
                foreach (TraitInstance trait in e.NewItems)
                {
                    trait.Fields.CollectionChanged += Fields_CollectionChanged;
                    foreach (var field in trait.Fields)
                        SubscribeToField(field);

                    History.Push(new TraitCollectionCommand(this, trait, isAdded: true));
                }
            }
            if (e.OldItems != null)
            {
                foreach (TraitInstance trait in e.OldItems)
                {
                    trait.Fields.CollectionChanged -= Fields_CollectionChanged;
                    foreach (var field in trait.Fields)
                        UnsubscribeFromField(field);

                    History.Push(new TraitCollectionCommand(this, trait, isAdded: false));
                }
            }
        }

        private void Fields_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            OnPropertyChanged(nameof(IsDirty));

            if (e.NewItems != null)
            {
                foreach (TraitFieldValue field in e.NewItems)
                    SubscribeToField(field);
            }
            if (e.OldItems != null)
            {
                foreach (TraitFieldValue field in e.OldItems)
                    UnsubscribeFromField(field);
            }
        }

        private void SubscribeToField(TraitFieldValue field)
        {
            field.ErrorsChanged += Field_ErrorsChanged;
            field.PropertyChanged += Field_PropertyChanged;
            field.FieldValueChanged += Field_FieldValueChanged;
        }

        private void UnsubscribeFromField(TraitFieldValue field)
        {
            field.ErrorsChanged -= Field_ErrorsChanged;
            field.PropertyChanged -= Field_PropertyChanged;
            field.FieldValueChanged -= Field_FieldValueChanged;
        }

        private void Field_FieldValueChanged(object? sender, FieldValueChangedEventArgs e)
        {
            if (sender is TraitFieldValue field)
                History.Push(new FieldValueChangeCommand(field, e.OldValue, e.NewValue));
        }

        private void Field_ErrorsChanged(object? sender, DataErrorsChangedEventArgs e)
        {
            OnPropertyChanged(nameof(HasErrors));
        }

        private void Field_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(TraitFieldValue.Value) || e.PropertyName == nameof(TraitFieldValue.IsDirty))
                OnPropertyChanged(nameof(IsDirty));
        }
    }
}
