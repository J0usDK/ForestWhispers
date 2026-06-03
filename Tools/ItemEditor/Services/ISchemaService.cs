using ItemEditor.Models.Schema;

namespace ItemEditor.Services;

internal interface ISchemaService
{
    Task<ItemTraitsSchema> LoadSchemaAsync(string filePath, CancellationToken cancellationToken = default);
}