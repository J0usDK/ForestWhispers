using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;
using ItemEditor.Models.Item;

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
    }
}
