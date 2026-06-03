using System.Collections.ObjectModel;
using ItemEditor.Core;

namespace ItemEditor.Models.Item;

internal sealed class TraitInstance : ViewModelBase
{
    public string Id { get; init; } = string.Empty;
    public ObservableCollection<TraitFieldValue> Fields { get; } = [];

    public TraitInstance Clone()
    {
        var clone = new TraitInstance { Id = this.Id };
        foreach (var field in Fields)
            clone.Fields.Add(field.Clone());
        return clone;
    }
}
