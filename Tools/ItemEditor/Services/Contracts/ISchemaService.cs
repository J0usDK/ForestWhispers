using ItemEditor.Models.Schema;

namespace ItemEditor.Services.Contracts;

internal interface ISchemaService
{
    Task<ItemTraitsSchema> LoadSchemaAsync(string filePath, CancellationToken cancellationToken = default);
}