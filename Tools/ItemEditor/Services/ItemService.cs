using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;
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
                    fieldsNode[field.Name] = JsonValue.Create(field.Value);
                traitsNode[trait.Id] = fieldsNode;
            }
            rootNode["traits"] = traitsNode;

            var options = new JsonSerializerOptions { WriteIndented = true };
            string jsonString = rootNode.ToJsonString(options);

            File.WriteAllText(filePath, jsonString);
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
