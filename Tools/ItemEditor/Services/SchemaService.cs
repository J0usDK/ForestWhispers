using System.IO;
using System.Text.Json;
using ItemEditor.Models.Schema;
using ItemEditor.Services.Contracts;

namespace ItemEditor.Services;

internal sealed class SchemaService : ISchemaService
{
    private readonly JsonSerializerOptions _serializerOptions = new()
    {
        ReadCommentHandling = JsonCommentHandling.Skip,
        AllowTrailingCommas = true
    };

    public async Task<ItemTraitsSchema> LoadSchemaAsync(string filePath, CancellationToken cancellationToken = default)
    {
        if (!File.Exists(filePath))
            throw new FileNotFoundException($"Schema file not found: {filePath}");

        await using FileStream stream = File.OpenRead(filePath);

        var schema = await JsonSerializer.DeserializeAsync<ItemTraitsSchema>(
            stream,
            _serializerOptions,
            cancellationToken).ConfigureAwait(false);

        return schema ?? throw new InvalidDataException("Failed to deserialize schema. File might be empty or invalid.");
    }
}
