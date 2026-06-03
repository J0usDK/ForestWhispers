using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;

namespace ItemEditor.Services;

internal interface IItemService
{
    ItemModel CreateNewItem(string id);
    Task SaveItemAsync(ItemModel item, string filePath, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<ItemModel>> LoadAllItemsParallelAsync(string directoryPath, ItemTraitsSchema schema, CancellationToken cancellationToken = default);

    TraitInstance CreateTraitInstance(TraitDefinition schemaDefinition);
}
