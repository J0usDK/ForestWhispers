using ItemEditor.Models.Contracts;
using ItemEditor.Services.Contracts;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace ItemEditor.Services;

internal sealed class ItemIDRegistryService : INotifyPropertyChanged, IItemIDRegistryService
{
    public event PropertyChangedEventHandler? PropertyChanged;
    public event Action<string>? ItemIDCollisionStateChanged;

    private readonly ConcurrentDictionary<string, HashSet<IItemData>> _registry = new(StringComparer.OrdinalIgnoreCase);

    private bool _hasAnyConflicts;
    public bool HasAnyConflicts => _hasAnyConflicts;
    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }

    public void Register(IItemData item)
    {
        if (item == null || string.IsNullOrWhiteSpace(item.ItemID)) return;

        var set = _registry.GetOrAdd(item.ItemID, _ => []);
        lock (set)
        {
            int previousCount = set.Count;
            if (set.Add(item) && set.Count == 2)
            {
                _hasAnyConflicts = true;
                OnPropertyChanged(nameof(HasAnyConflicts));
                ItemIDCollisionStateChanged?.Invoke(item.ItemID);
            }
        }
    }

    public void Unregister(IItemData item)
    {
        if (item == null || string.IsNullOrWhiteSpace(item.ItemID)) return;

        if (_registry.TryGetValue(item.ItemID, out var set))
        {
            lock (set)
            {
                if (set.Remove(item))
                {
                    if (set.Count == 1)
                    {
                        _hasAnyConflicts = false;
                        ItemIDCollisionStateChanged?.Invoke(item.ItemID);
                    }

                    if (set.Count == 0)
                        _registry.TryRemove(item.ItemID, out _);
                }
            }
        }
    }

    public void UpdateItemID(IItemData item, string? oldID, string? newID)
    {
        if (string.Equals(oldID, newID, StringComparison.OrdinalIgnoreCase)) return;

        bool oldStatusChanged = false;
        bool newStatusChanged = false;

        // Unregister old ID
        if (!string.IsNullOrWhiteSpace(oldID) && _registry.TryGetValue(oldID, out var oldSet))
        {
            lock (oldSet)
            {
                if (oldSet.Remove(item))
                {
                    if (oldSet.Count == 1)
                        oldStatusChanged = true;
                    else if (oldSet.Count == 0)
                        _registry.TryRemove(oldID, out _);
                }
            }
        }

        // Register new ID
        if (!string.IsNullOrWhiteSpace(newID))
        {
            var newSet = _registry.GetOrAdd(newID, _ => []);
            lock (newSet)
            {
                if (newSet.Add(item) && newSet.Count == 2)
                    newStatusChanged = true;
            }
        }

        bool oldHasConflicts = _hasAnyConflicts;
        _hasAnyConflicts = _registry.Values.Any(set => set.Count > 1);
        if (oldHasConflicts != _hasAnyConflicts)
            OnPropertyChanged(nameof(HasAnyConflicts));

        if (oldID != null && oldStatusChanged) ItemIDCollisionStateChanged?.Invoke(oldID);
        if (newID != null && newStatusChanged) ItemIDCollisionStateChanged?.Invoke(newID);
    }

    public bool IsIDDuplicate(string? id)
    {
        if (string.IsNullOrWhiteSpace(id)) return false;
        return _registry.TryGetValue(id, out var set) && set.Count > 1;
    }

    public IEnumerable<IItemData> GetItemsByID(string id)
    {
        if (string.IsNullOrWhiteSpace(id)) return [];

        if (_registry.TryGetValue(id, out var set))
        {
            lock (set)
            {
                return [.. set];
            }
        }
        return [];
    }
}