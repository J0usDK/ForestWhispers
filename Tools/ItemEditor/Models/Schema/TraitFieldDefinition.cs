using System.Text.Json;
using System.Text.Json.Serialization;

namespace ItemEditor.Models.Schema;
internal sealed class TraitFieldDefinition
{
    [JsonPropertyName("name")]
    public string Name { get; init; } = string.Empty;

    [JsonPropertyName("type")]
    public string Type { get; init; } = string.Empty;

    [JsonPropertyName("default")]
    public JsonElement DefaultValue { get; init; }

    [JsonPropertyName("min")]
    public float? MinValue { get; init; }

    [JsonPropertyName("max")]
    public float? MaxValue { get; init; }
}
