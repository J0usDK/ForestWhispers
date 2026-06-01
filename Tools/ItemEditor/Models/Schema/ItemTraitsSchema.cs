using System.Text.Json.Serialization;

namespace ItemEditor.Models.Schema
{
    internal class ItemTraitsSchema
    {
        [JsonPropertyName("version")]
        public string Version { get; init; } = string.Empty;

        [JsonPropertyName("traits")]
        public IReadOnlyList<TraitDefinition> Traits { get; init; } = Array.Empty<TraitDefinition>();
    }
}
