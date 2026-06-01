using System.Text.Json.Serialization;

namespace ItemEditor.Models.Schema
{
    internal class TraitDefinition
    {
        [JsonPropertyName("id")]
        public string Id { get; init; } = string.Empty;

        [JsonPropertyName("displayName")]
        public string DisplayName { get; init; } = string.Empty;

        [JsonPropertyName("description")]
        public string Description { get; init; } = string.Empty;

        [JsonPropertyName("fields")]
        public IReadOnlyList<TraitFieldDefinition> Fields { get; init; } = Array.Empty<TraitFieldDefinition>();
    }
}
