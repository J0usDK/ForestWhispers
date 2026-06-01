using System.IO;
using System.Text.Json;
using ItemEditor.Models.Schema;

namespace ItemEditor.Services
{
    internal class SchemaService : ISchemaService
    {
        public ItemTraitsSchema LoadSchema(string filePath)
        {
            if (!File.Exists(filePath))
                throw new FileNotFoundException($"Schema file not found: {filePath}");

            string json = File.ReadAllText(filePath);
            var option = new JsonSerializerOptions
            {
                ReadCommentHandling = JsonCommentHandling.Skip,
                AllowTrailingCommas = true
            };

            var schema = JsonSerializer.Deserialize<ItemTraitsSchema>(json, option);
            return schema ?? throw new InvalidDataException("Filed to deserialize schema. File might be empty or invalid.");
        }
    }
}
