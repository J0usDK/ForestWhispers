using System.Text.Json.Serialization;

namespace ItemEditor.Models.Schema;

internal sealed class ItemTypeDefinition
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("value")]
    public int Value { get; set; }
}
