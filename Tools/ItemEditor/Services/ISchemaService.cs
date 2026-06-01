using ItemEditor.Models.Schema;

namespace ItemEditor.Services
{
    internal interface ISchemaService
    {
        ItemTraitsSchema LoadSchema(string filePath);
    }
}
