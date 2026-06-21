using CommunityToolkit.Mvvm.ComponentModel;
using ItemEditor.Core.Types;
using ItemEditor.Core.UndoRedo;
using ItemEditor.Core.Validation.Context;
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

    public MetadataFieldValue Description { get; } = new() { FieldType = MetadataFieldType.None };
    public MetadataFieldValue GeometryPath { get; }
    public MetadataFieldValue IconPath { get; }

    public bool IsDirty => History.IsDirty;

    public bool HasErrors => _invalidFields.Count > 0;

    private readonly HashSet<INotifyDataErrorInfo> _invalidFields = [];

    private readonly ObservableCollection<TraitInstance> _traits = [];
    public ReadOnlyObservableCollection<TraitInstance> Traits { get; }

    public ItemModel(IPathValidationContext? pathValidationContext = null)
    {
        GeometryPath = new() { FieldType = MetadataFieldType.GeometryPath, PathValidationContext = pathValidationContext };
        IconPath = new() { FieldType = MetadataFieldType.IconPath, PathValidationContext = pathValidationContext };

        Traits = new ReadOnlyObservableCollection<TraitInstance>(_traits);
        _traits.CollectionChanged += Traits_CollectionChanged;
        History.StateChanged += () => OnPropertyChanged(nameof(IsDirty));

        HookMetadataField(Description, nameof(Description), val => Description.Value = val);
        HookMetadataField(GeometryPath, nameof(GeometryPath), val => GeometryPath.Value = val);
        HookMetadataField(IconPath, nameof(IconPath), val => IconPath.Value = val);
    }

    public IItemModel Clone(string newID, IPathValidationContext? pathValidationContext = null)
    {
        var clone = new ItemModel(pathValidationContext) { ItemID = newID };
        clone.Description.Value = this.Description.Value;
        clone.GeometryPath.Value = this.GeometryPath.Value;
        clone.IconPath.Value = this.IconPath.Value;

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

    partial void OnItemIDChanged(string? oldValue, string newValue)
    {
        if (oldValue == null || oldValue == newValue) return;

        ItemIDChanged?.Invoke(this, (oldValue, newValue));
        History.Push(new ItemMetadataChangeCommand(this, nameof(ItemID), val => ItemID = val, oldValue, newValue));
    }

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

    private void HookMetadataField(MetadataFieldValue field, string propertyName, Action<string> setter)
    {
        field.FieldValueChanged += (sender, e) =>
        {
            if (e.OldValue != null && e.NewValue != null)
                History.Push(new ItemMetadataChangeCommand(this, propertyName, setter, (string)e.OldValue, (string)e.NewValue));
        };

        field.ErrorsChanged += (sender, e) =>
        {
            if (field.HasErrors) _invalidFields.Add(field);
            else _invalidFields.Remove(field);
            OnPropertyChanged(nameof(HasErrors));
        };
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
