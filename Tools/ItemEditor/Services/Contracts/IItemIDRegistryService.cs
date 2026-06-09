using ItemEditor.Models.Contracts;
using System.ComponentModel;

namespace ItemEditor.Services.Contracts;

internal interface IItemIDRegistryService : INotifyPropertyChanged
{
    bool HasAnyConflicts { get; }
    event Action<string>? ItemIDCollisionStateChanged;

    void Register(IItemData item);
    void Unregister(IItemData item);
    void UpdateItemID(IItemData item, string? oldID, string? newID);
    bool IsIDDuplicate(string? id);

    IEnumerable<IItemData> GetItemsByID(string id);
}
