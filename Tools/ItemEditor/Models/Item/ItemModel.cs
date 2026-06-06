using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using ItemEditor.Core;
using ItemEditor.Core.UndoRedo;

namespace ItemEditor.Models.Item;

internal sealed class ItemModel : ViewModelBase
{
    public UndoRedoManager History { get; } = new(50);

    public event EventHandler<(string OldID, string NewID)>? ItemIDChanged;

    private string _itemID = string.Empty;
    public string ItemID
    {
        get => _itemID;
        set
        {
            if (_itemID != value)
            {
                string oldID = _itemID;
                _itemID = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(IsDirty));
                ItemIDChanged?.Invoke(this, (oldID, value));
            }
        }
    }

    private string? _originalItemID;
    public string? OriginalItemID => _originalItemID;


    private List<string> _originalTraitIDs = [];
    public bool IsDirty
    {
        get
        {
            if (_originalItemID == null || ItemID != _originalItemID || Traits.Count != _originalTraitIDs.Count)
                return true;

            for (int i = 0; i < Traits.Count; i++)
                if (Traits[i].Id != _originalTraitIDs[i]) return true;

            foreach (var trait in Traits)
                foreach (var field in trait.Fields) if (field.IsDirty) return true;

            return false;
        }
    }

    public bool HasErrors
    {
        get
        {
            foreach (var trait in Traits)
                foreach (var field in trait.Fields) if (field.HasErrors) return true;

            return false;
        }
    }

    public ObservableCollection<TraitInstance> Traits { get; }

    public ItemModel()
    {

        Traits = [];
        Traits.CollectionChanged += Traits_CollectionChanged;
    }

    public ItemModel Clone(string newID)
    {
        var clone = new ItemModel { ItemID = newID };
        foreach (var trait in Traits)
            clone.Traits.Add(trait.Clone());
        return clone;
    }

    public void AcceptChanges()
    {
        _originalItemID = ItemID;
        _originalTraitIDs = [.. Traits.Select(t => t.Id)];

        foreach (var trait in Traits)
            foreach (var field in trait.Fields) field.AcceptChanges();

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
                foreach (var field in trait.Fields) SubscribeToField(field);
                History.Push(new TraitCollectionCommand(this, trait, isAdded: true));
            }
        }
        if (e.OldItems != null)
        {
            foreach (TraitInstance trait in e.OldItems)
            {
                trait.Fields.CollectionChanged -= Fields_CollectionChanged;
                foreach (var field in trait.Fields) UnsubscribeFromField(field);
                History.Push(new TraitCollectionCommand(this, trait, isAdded: false));
            }
        }
    }

    private void Fields_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
    {
        OnPropertyChanged(nameof(IsDirty));

        if (e.NewItems != null)
            foreach (TraitFieldValue field in e.NewItems) SubscribeToField(field);

        if (e.OldItems != null)
            foreach (TraitFieldValue field in e.OldItems) UnsubscribeFromField(field);
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
        if (sender is TraitFieldValue field && e.OldValue != null && e.NewValue != null)
            History.Push(new FieldValueChangeCommand(field, e.OldValue, e.NewValue));
    }

    private void Field_ErrorsChanged(object? sender, DataErrorsChangedEventArgs e) => OnPropertyChanged(nameof(HasErrors));

    private void Field_PropertyChanged(object? sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName is nameof(TraitFieldValue.Value) or nameof(TraitFieldValue.IsDirty))
            OnPropertyChanged(nameof(IsDirty));
    }
}
