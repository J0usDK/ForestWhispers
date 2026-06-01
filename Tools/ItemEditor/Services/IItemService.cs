using ItemEditor.Models.Item;
using ItemEditor.Models.Schema;

namespace ItemEditor.Services
{
    internal interface IItemService
    {
        ItemModel CreateNewItem(string id);
        void SaveItem(ItemModel item, string filePath);

        TraitInstance CreateTraitInstance(TraitDefinition schemaDefinition);
    }
}
