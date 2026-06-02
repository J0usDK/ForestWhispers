using System.Diagnostics;
using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Serialization.Metadata;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;

namespace ItemEditor.Services
{
    internal class ItemService : IItemService
    {
        public ItemModel CreateNewItem(string id)
        {
            return new ItemModel { ItemID = id };
        }

        public void SaveItem(ItemModel item, string filePath)
        {
            var rootNode = new JsonObject
            {
                ["ItemID"] = item.ItemID
            };

            var traitsNode = new JsonObject();
            foreach (var trait in item.Traits)
            {
                var fieldsNode = new JsonObject();
                foreach (var field in trait.Fields)
                {
                    string strValue = field.Value?.ToString() ?? string.Empty;
                    if (field.Type.ToLower() == "float")
                    {
                        if (float.TryParse(strValue, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out float fVal))
                            fieldsNode[field.Name] = JsonValue.Create(fVal);
                        else
                            fieldsNode[field.Name] = JsonValue.Create(0.0f);
                    }
                    else if (field.Type.ToLower() == "bool" || field.Type.ToLower() == "boolean")
                    {
                        if (bool.TryParse(strValue, out bool bVal))
                            fieldsNode[field.Name] = JsonValue.Create(bVal);
                        else
                            fieldsNode[field.Name] = JsonValue.Create(false);
                    }
                    else if (field.Type.ToLower() == "string")
                        fieldsNode[field.Name] = JsonValue.Create(strValue);
                }
                traitsNode[trait.Id] = fieldsNode;
            }
            rootNode["traits"] = traitsNode;

            var options = new JsonSerializerOptions
            { 
                WriteIndented = true,
                TypeInfoResolver = new DefaultJsonTypeInfoResolver()
            };
            string jsonString = rootNode.ToJsonString(options);

            File.WriteAllText(filePath, jsonString);
        }

        public IEnumerable<ItemModel> LoadAllItems(string directoryPath, ItemTraitsSchema schema)
        {
            var items = new List<ItemModel>();

            if (string.IsNullOrWhiteSpace(directoryPath) || !Directory.Exists(directoryPath))
                return items;

            foreach (var file in Directory.GetFiles(directoryPath, "*.json"))
            {
                var item = LoadSingleItem(file, schema);
                if (item != null)
                    items.Add(item);
            }
            return items;
        }

        private ItemModel? LoadSingleItem(string filePath, ItemTraitsSchema schema)
        {
            try
            {
                string json = File.ReadAllText(filePath);
                var rootNode = JsonNode.Parse(json)?.AsObject();
                if (rootNode == null)
                    return null;
                return ParseItemModel(rootNode, filePath, schema);
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
                    if (traitInstance != null)
                        item.Traits.Add(traitInstance);
                }
            }
            return item;
        }

        private TraitInstance? ParseTraitInstance(string traitID, JsonObject? fieldObject, ItemTraitsSchema schema)
        {
            if (fieldObject == null)
                return null;

            var schemaDefinition = schema.Traits.FirstOrDefault(t => t.Id == traitID);
            if (schemaDefinition == null)
                return null;

            var traitInstance = CreateTraitInstance(schemaDefinition);
            foreach (var field in traitInstance.Fields)
            {
                if (fieldObject.TryGetPropertyValue(field.Name, out var savedValueNode) && savedValueNode != null)
                    AssignFieldValue(field, savedValueNode);
            }
            return traitInstance;
        }

        private void AssignFieldValue(TraitFieldValue field, JsonNode savedValueNode)
        {
            string savedString = savedValueNode.ToString();
            string fieldType = field.Type.ToLower();

            if (fieldType == "float" && float.TryParse(savedString, out float fVal))
                field.Value = fVal;
            else if (fieldType == "string")
                field.Value = savedString;
            else if ((fieldType == "bool" || fieldType == "boolean") && bool.TryParse(savedString, out bool bVal))
                field.Value = bVal;
        }

        public TraitInstance CreateTraitInstance(TraitDefinition schemaDefinition)
        {
            var instance = new TraitInstance { Id = schemaDefinition.Id };
            foreach (var fieldDefinition in schemaDefinition.Fields)
            {
                object defaultValue = 0.0f; // Default
                switch (fieldDefinition.DefaultValue.ValueKind)
                {
                    case JsonValueKind.Number:
                        defaultValue = fieldDefinition.DefaultValue.GetSingle();
                        break;
                    case JsonValueKind.String:
                        defaultValue = fieldDefinition.DefaultValue.GetString() ?? string.Empty;
                        break;
                    case JsonValueKind.True:
                    case JsonValueKind.False:
                        defaultValue = fieldDefinition.DefaultValue.GetBoolean();
                        break;
                }

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
}
