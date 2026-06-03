using System.Diagnostics;
using System.Globalization;
using System.Collections.Concurrent;
using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Core;

namespace ItemEditor.Services;

internal sealed class ItemService : IItemService
{
    private readonly JsonSerializerOptions _jsonOptions = new() { WriteIndented = true };

    public ItemModel CreateNewItem(string id) => new() { ItemID = id };

    public async Task SaveItemAsync(ItemModel item, string filePath, CancellationToken cancellationToken = default)
    {
        var traitsNode = new JsonObject();
        foreach (var trait in item.Traits)
        {
            var fieldsNode = new JsonObject();
            foreach (var field in trait.Fields)
            {
                string strValue = field.Value?.ToString() ?? string.Empty;
                var dataType = FieldTypeParser.Parse(field.Type);

                fieldsNode[field.Name] = dataType switch
                {
                    FieldDataType.Float => float.TryParse(strValue, NumberStyles.Float, CultureInfo.InvariantCulture, out float fVal) ? JsonValue.Create(fVal) : JsonValue.Create(0.0f),
                    FieldDataType.Boolean => bool.TryParse(strValue, out bool bVal) ? JsonValue.Create(bVal) : JsonValue.Create(false),
                    FieldDataType.String => JsonValue.Create(strValue),
                    _ => JsonValue.Create(strValue)
                };
            }
            traitsNode[trait.Id] = fieldsNode;
        }

        var rootNode = new JsonObject
        {
            ["ItemID"] = item.ItemID,
            ["traits"] = traitsNode
        };

        if (item.OriginalItemID != null && item.OriginalItemID != item.ItemID)
        {
            string? directory = Path.GetDirectoryName(filePath);
            if (!string.IsNullOrEmpty(directory))
            {
                string oldFilePath = Path.Combine(directory, $"{item.OriginalItemID}.json");
                if (File.Exists(oldFilePath))
                    File.Delete(oldFilePath);
            }
        }

        await using var stream = File.Create(filePath);
        await JsonSerializer.SerializeAsync(stream, rootNode, _jsonOptions, cancellationToken).ConfigureAwait(false);

        item.AcceptChanges();
    }

    public async Task<IReadOnlyList<ItemModel>> LoadAllItemsParallelAsync(string directoryPath, ItemTraitsSchema schema, CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrWhiteSpace(directoryPath) || !Directory.Exists(directoryPath))
            return [];

        var files = Directory.EnumerateFiles(directoryPath, "*.json");
        var items = new ConcurrentBag<ItemModel>();

        var options = new ParallelOptions
        {
            MaxDegreeOfParallelism = Environment.ProcessorCount,
            CancellationToken = cancellationToken
        };

        await Parallel.ForEachAsync(files, options, async (file, ct) =>
        {
            var item = await LoadSingleItemAsync(file, schema, ct).ConfigureAwait(false);
            if (item != null)
                items.Add(item);
        }).ConfigureAwait(false);

        return [.. items];
    }

    private async Task<ItemModel?> LoadSingleItemAsync(string filePath, ItemTraitsSchema schema, CancellationToken cancellationToken)
    {
        try
        {
            await using var stream = File.OpenRead(filePath);
            var rootNode = await JsonNode.ParseAsync(stream, cancellationToken: cancellationToken).ConfigureAwait(false);

            if (rootNode?.AsObject() is not { } rootObject) return null;
            return ParseItemModel(rootObject, filePath, schema);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"[ERROR] Failed to load item {filePath}: {ex.Message}");
            return null;
        }
    }

    private ItemModel ParseItemModel(JsonObject rootNode, string filePath, ItemTraitsSchema schema)
    {
        string itemID = rootNode["itemID"]?.ToString() ?? Path.GetFileNameWithoutExtension(filePath);
        var item = new ItemModel { ItemID = itemID };

        if (rootNode.TryGetPropertyValue("traits", out var traitNode) && traitNode is JsonObject traitsObject)
        {
            foreach (var traitProp in traitsObject)
            {
                var traitInstance = ParseTraitInstance(traitProp.Key, traitProp.Value as JsonObject, schema);
                if (traitInstance != null) item.Traits.Add(traitInstance);
            }
        }

        item.AcceptChanges();
        item.History.Clear();
        return item;
    }

    private TraitInstance? ParseTraitInstance(string traitID, JsonObject? fieldObject, ItemTraitsSchema schema)
    {
        if (fieldObject == null) return null;

        var schemaDefinition = schema.Traits.FirstOrDefault(t => t.Id == traitID);
        if (schemaDefinition == null) return null;

        var traitInstance = CreateTraitInstance(schemaDefinition);
        foreach (var field in traitInstance.Fields)
        {
            if (fieldObject.TryGetPropertyValue(field.Name, out var savedValueNode) && savedValueNode != null)
                AssignFieldValue(field, savedValueNode);
        }
        return traitInstance;
    }

    private static void AssignFieldValue(TraitFieldValue field, JsonNode savedValueNode)
    {
        string savedString = savedValueNode.ToString();
        var dataType = FieldTypeParser.Parse(field.Type);

        field.Value = dataType switch
        {
            FieldDataType.Float => float.TryParse(savedString, NumberStyles.Float, CultureInfo.InvariantCulture, out float fVal) ? JsonValue.Create(fVal) : JsonValue.Create(0.0f),
            FieldDataType.Boolean => bool.TryParse(savedString, out bool bVal) ? JsonValue.Create(bVal) : JsonValue.Create(false),
            FieldDataType.String => JsonValue.Create(savedString),
            _ => JsonValue.Create(savedString)
        };
    }

    public TraitInstance CreateTraitInstance(TraitDefinition schemaDefinition)
    {
        var instance = new TraitInstance { Id = schemaDefinition.Id };
        foreach (var fieldDefinition in schemaDefinition.Fields)
        {
            object defaultValue = fieldDefinition.DefaultValue.ValueKind switch
            {
                JsonValueKind.Number => fieldDefinition.DefaultValue.GetSingle(),
                JsonValueKind.String => fieldDefinition.DefaultValue.GetString() ?? string.Empty,
                JsonValueKind.True => true,
                JsonValueKind.False => false,
                _ => 0.0f
            };

            instance.Fields.Add(new TraitFieldValue
            {
                Name = fieldDefinition.Name,
                Type = fieldDefinition.Type,
                Min = fieldDefinition.MinValue,
                Max = fieldDefinition.MaxValue,
                Value = defaultValue
            });
        }
        return instance;
    }
}
