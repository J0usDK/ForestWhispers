using ItemEditor.Core.Types;
using ItemEditor.Models.Contracts;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;
using ItemEditor.Services.Contracts;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace ItemEditor.Services;

internal sealed class ItemService : IItemService
{
    private readonly JsonSerializerOptions _jsonOptions = new() { WriteIndented = true };

    public IItemModel CreateNewItem(string id)
    {
        var item = new ItemModel { ItemID = id };
        item.ClearHistory();
        return item;
    }

    public async Task SaveItemAsync(IItemData item, string filePath, CancellationToken cancellationToken = default)
    {
        DeleteOldFileIfNeeded(item, filePath);

        await using var stream = new FileStream(filePath, FileMode.Create, FileAccess.Write, FileShare.None, 4096, useAsync: true);

        var writerOptions = new JsonWriterOptions { Indented = true };
        await using var writer = new Utf8JsonWriter(stream, writerOptions);

        writer.WriteStartObject();
        writer.WriteString("ItemID", item.ItemID);
        writer.WriteString("Description", item.Description.Value);
        writer.WriteString("GeometryPath", item.GeometryPath.Value);
        writer.WriteString("IconPath", item.IconPath.Value);
        writer.WriteStartObject("traits");

        foreach (var trait in item.Traits)
        {
            writer.WriteStartObject(trait.Id);
            foreach (var field in trait.Fields)
            {
                writer.WritePropertyName(field.Name);
                WriteFieldValue(writer, field.Value, field.DataType);
            }
            writer.WriteEndObject();
        }
        writer.WriteEndObject();
        writer.WriteEndObject();

        await writer.FlushAsync(cancellationToken).ConfigureAwait(false);
    }

    public async Task<IReadOnlyList<IItemModel>> LoadAllItemsParallelAsync(string directoryPath, ItemTraitsSchema schema, CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrWhiteSpace(directoryPath) || !Directory.Exists(directoryPath))
            return [];

        var files = Directory.EnumerateFiles(directoryPath, "*.json");
        var items = new ConcurrentBag<IItemModel>();

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

    private async Task<IItemModel?> LoadSingleItemAsync(string filePath, ItemTraitsSchema schema, CancellationToken cancellationToken)
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

    private IItemModel ParseItemModel(JsonObject rootNode, string filePath, ItemTraitsSchema schema)
    {
        string itemID = rootNode["ItemID"]?.ToString() ?? Path.GetFileNameWithoutExtension(filePath);
        string description = rootNode["Description"]?.ToString() ?? String.Empty;
        string geometryPath = rootNode["GeometryPath"]?.ToString() ?? String.Empty;
        string iconPath = rootNode["IconPath"]?.ToString() ?? String.Empty;

        IItemModel item = new ItemModel { ItemID = itemID };

        item.Description.Value = description;
        item.GeometryPath.Value = geometryPath;
        item.IconPath.Value = iconPath;

        if (rootNode.TryGetPropertyValue("traits", out var traitNode) && traitNode is JsonObject traitsObject)
        {
            foreach (var traitProp in traitsObject)
            {
                var traitInstance = ParseTraitInstance(traitProp.Key, traitProp.Value as JsonObject, schema);
                if (traitInstance != null) item.AddTrait(traitInstance);
            }
        }

        item.AcceptChanges();
        item.ClearHistory();
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

        field.Value = field.DataType switch
        {
            FieldDataType.Float => float.TryParse(savedString, NumberStyles.Float, CultureInfo.InvariantCulture, out float fVal) ? fVal : 0.0f,
            FieldDataType.Boolean => bool.TryParse(savedString, out bool bVal) ? bVal : false,
            FieldDataType.String => savedString,
            _ => savedString
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

    private static void WriteFieldValue(Utf8JsonWriter writer, object? value, FieldDataType fieldType)
    {
        switch (value)
        {
            case float f:
                writer.WriteNumberValue(f);
                break;
            case bool b:
                writer.WriteBooleanValue(b);
                break;
            case string s:
                WriteParsedStringValue(writer, s, fieldType);
                break;
            default:
                writer.WriteNullValue();
                break;
        }
    }

    private static void WriteParsedStringValue(Utf8JsonWriter writer, string strValue, FieldDataType dataType)
    {
        switch (dataType)
        {
            case FieldDataType.Float:
                if (float.TryParse(strValue, NumberStyles.Float, CultureInfo.InvariantCulture, out float fValue))
                    writer.WriteNumberValue(fValue);
                else
                    writer.WriteNumberValue(0.0f);
                break;
            case FieldDataType.Boolean:
                if (bool.TryParse(strValue, out bool bValue))
                    writer.WriteBooleanValue(bValue);
                else
                    writer.WriteBooleanValue(false);
                break;
            default:
                writer.WriteStringValue(strValue);
                break;
        }
    }

    private static void DeleteOldFileIfNeeded(IItemData item, string newFilePath)
    {
        if (item.OriginalItemID != null && item.OriginalItemID != item.ItemID)
        {
            string? directory = Path.GetDirectoryName(newFilePath);
            if (!string.IsNullOrEmpty(directory))
            {
                string oldFilePath = Path.Combine(directory, $"{item.OriginalItemID}.json");
                if (File.Exists(oldFilePath))
                    File.Delete(oldFilePath);
            }
        }
    }
}
