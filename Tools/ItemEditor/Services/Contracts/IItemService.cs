using ItemEditor.Models.Contracts;
using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;

namespace ItemEditor.Services.Contracts;

internal interface IItemService
{
    IItemModel CreateNewItem(string id);
    Task SaveItemAsync(IItemData item, string filePath, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<IItemModel>> LoadAllItemsParallelAsync(string directoryPath, ItemTraitsSchema schema, CancellationToken cancellationToken = default);

    TraitInstance CreateTraitInstance(TraitDefinition schemaDefinition);
}
