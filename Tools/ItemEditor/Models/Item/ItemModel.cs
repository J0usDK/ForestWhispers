using ItemEditor.Core;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace ItemEditor.Models.Item
{
    internal class ItemModel : ViewModelBase
    {
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
                    _isStructurallyDirty = true;
                    OnPropertyChanged(nameof(IsDirty));
                }
            }
        }

        private bool _isStructurallyDirty;
        public bool IsDirty => _isStructurallyDirty || Traits.Any(t => t.Fields.Any(f => f.IsDirty));

        public ObservableCollection<TraitInstance> Traits { get; }
        public bool HasErrors => Traits.Any(t => t.Fields.Any(f => f.HasErrors));

        public ItemModel()
        {
            Traits = new ObservableCollection<TraitInstance>();
            Traits.CollectionChanged += Traits_CollectionChanged;
        }

        public void AcceptChanges()
        {
            _isStructurallyDirty = false;
            foreach (var trait in Traits)
            {
                foreach (var field in trait.Fields)
                    field.AcceptChanges();
            }
            OnPropertyChanged(nameof(IsDirty));
        }

        private void Traits_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            _isStructurallyDirty = true;
            OnPropertyChanged(nameof(IsDirty));
            OnPropertyChanged(nameof(HasErrors));

            if (e.NewItems != null)
            {
                foreach (TraitInstance trait in e.NewItems)
                {
                    trait.Fields.CollectionChanged += Fields_CollectionChanged;
                    foreach (var field in trait.Fields)
                        SubscribeToField(field);
                }
            }
            if (e.OldItems != null)
            {
                foreach (TraitInstance trait in e.OldItems)
                {
                    trait.Fields.CollectionChanged -= Fields_CollectionChanged;
                    foreach (var field in trait.Fields)
                        UnsubscribeFromField(field);
                }
            }
        }

        private void Fields_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            _isStructurallyDirty = true;
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
        }

        private void UnsubscribeFromField(TraitFieldValue field)
        {
            field.ErrorsChanged -= Field_ErrorsChanged;
            field.PropertyChanged -= Field_PropertyChanged;
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
