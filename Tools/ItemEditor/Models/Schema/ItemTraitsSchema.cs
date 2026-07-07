using System.Text.Json.Serialization;

namespace ItemEditor.Models.Schema;

internal sealed class ItemTraitsSchema
{
    [JsonPropertyName("version")]
    public string Version { get; init; } = string.Empty;

    [JsonPropertyName("itemTypes")]
    public IReadOnlyList<ItemTypeDefinition> ItemTypes { get; init; } = [];

    [JsonPropertyName("traits")]
    public IReadOnlyList<TraitDefinition> Traits { get; init; } = [];
}

