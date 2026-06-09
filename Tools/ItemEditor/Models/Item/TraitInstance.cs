using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.ObjectModel;

namespace ItemEditor.Models.Item;

internal sealed partial class TraitInstance : ObservableObject
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
