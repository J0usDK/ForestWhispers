using ItemEditor.Models.Item;

namespace ItemEditor.Services
{
    internal interface IItemService
    {
        ItemModel CreateNewItem(string id);
        void SaveItem(ItemModel item, string filePath);
    }
}
