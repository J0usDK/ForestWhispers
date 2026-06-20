using CommunityToolkit.Mvvm.ComponentModel;
using ItemEditor.Core.UndoRedo;
using ItemEditor.Models.Contracts;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace ItemEditor.Models.Item;

internal sealed partial class ItemModel : ObservableObject, IItemModel
{
    public UndoRedoManager History { get; } = new(50);

    public event EventHandler<(string OldID, string NewID)>? ItemIDChanged;

    [ObservableProperty]
    private bool _isIDDuplicate;

    [ObservableProperty]
    private string _itemID = string.Empty;

    private string? _originalItemID;
    public string? OriginalItemID => _originalItemID;

    [ObservableProperty]
    private string _description = string.Empty;

    [ObservableProperty]
    private string _geometryPath = string.Empty;

    [ObservableProperty]
    private string _iconPath = string.Empty;

    public bool IsDirty => History.IsDirty;

    [ObservableProperty]
    public bool _hasErrors;

    private readonly HashSet<TraitFieldValue> _invalidFields = [];

    private readonly ObservableCollection<TraitInstance> _traits = [];
    public ReadOnlyObservableCollection<TraitInstance> Traits { get; }

    partial void OnItemIDChanged(string? oldValue, string newValue)
    {
        if (oldValue == null || oldValue == newValue) return;

        ItemIDChanged?.Invoke(this, (oldValue, newValue));
        History.Push(new ItemMetadataChangeCommand(this, nameof(ItemID), val => ItemID = val, oldValue, newValue));
    }

    partial void OnDescriptionChanged(string? oldValue, string newValue)
    {
        if (oldValue == null || oldValue == newValue) return;

        History.Push(new ItemMetadataChangeCommand(this, nameof(Description), val => Description = val, oldValue, newValue));
    }

    partial void OnGeometryPathChanged(string? oldValue, string newValue)
    {
        if (oldValue == null || oldValue == newValue) return;

        History.Push(new ItemMetadataChangeCommand(this, nameof(GeometryPath), val =>  GeometryPath = val, oldValue, newValue));
    }

    partial void OnIconPathChanged(string? oldValue, string newValue)
    {
        if (oldValue == null || oldValue == newValue) return;

        History.Push(new ItemMetadataChangeCommand(this, nameof(IconPath), val =>  IconPath = val, oldValue, newValue));
    }

    public ItemModel()
    {

        Traits = new ReadOnlyObservableCollection<TraitInstance>(_traits);
        _traits.CollectionChanged += Traits_CollectionChanged;
        History.StateChanged += () => OnPropertyChanged(nameof(IsDirty));
    }

    public IItemModel Clone(string newID)
    {
        var clone = new ItemModel { ItemID = newID, Description=this.Description, GeometryPath=this.GeometryPath, IconPath=this.IconPath };
        foreach (var trait in Traits)
            clone.AddTrait(trait.Clone());
        clone.ClearHistory();
        return clone;
    }

    public void AcceptChanges()
    {
        _originalItemID = ItemID;

        foreach (var trait in Traits)
            foreach (var field in trait.Fields) field.AcceptChanges();

        History.MarkAsSaved();
    }

    public void AddTrait(TraitInstance trait)
    {
        _traits.Add(trait);
    }

    public void RemoveTrait(TraitInstance trait)
    {
        _traits.Remove(trait);
    }

    public bool CanUndo() => History.CanUndo;
    public void Undo() => History.Undo();
    public bool CanRedo() => History.CanRedo;
    public void Redo() => History.Redo();
    public void ClearHistory() => History.Clear();

    private void Traits_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
    {
        if (e.NewItems != null)
        {
            foreach (TraitInstance trait in e.NewItems)
            {
                trait.Fields.CollectionChanged += Fields_CollectionChanged;
                foreach (var field in trait.Fields) HookField(field);
                History.Push(new TraitCollectionCommand(this, trait, isAdded: true));
            }
        }
        if (e.OldItems != null)
        {
            foreach (TraitInstance trait in e.OldItems)
            {
                trait.Fields.CollectionChanged -= Fields_CollectionChanged;
                foreach (var field in trait.Fields) UnhookField(field);
                History.Push(new TraitCollectionCommand(this, trait, isAdded: false));
            }
        }
    }

    private void Fields_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
    {
        if (e.NewItems != null)
        {
            foreach (TraitFieldValue field in e.NewItems)
                HookField(field);
        }
        if (e.OldItems != null)
        {
            foreach (TraitFieldValue field in e.OldItems)
                UnhookField(field);
        }
    }

    private void HookField(TraitFieldValue field)
    {
        field.ErrorsChanged += Field_ErrorsChanged;
        field.FieldValueChanged += Field_FieldValueChanged;
        if (field.HasErrors) _invalidFields.Add(field);
    }

    private void UnhookField(TraitFieldValue field)
    {
        field.ErrorsChanged -= Field_ErrorsChanged;
        field.FieldValueChanged -= Field_FieldValueChanged;
        if (_invalidFields.Remove(field)) OnPropertyChanged(nameof(HasErrors));
    }

    private void Field_FieldValueChanged(object? sender, FieldValueChangedEventArgs e)
    {
        if (sender is TraitFieldValue field && e.OldValue != null && e.NewValue != null)
            History.Push(new FieldValueChangeCommand(field, e.OldValue, e.NewValue));
    }

    private void Field_ErrorsChanged(object? sender, DataErrorsChangedEventArgs e)
    {
        if (sender is TraitFieldValue field)
        {
            if (field.HasErrors) _invalidFields.Add(field);
            else _invalidFields.Remove(field);
            OnPropertyChanged(nameof(HasErrors));
        }
    }
}
