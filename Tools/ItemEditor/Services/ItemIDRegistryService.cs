using ItemEditor.Models.Item;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace ItemEditor.Services;

internal sealed class ItemIDRegistryService : INotifyPropertyChanged
{
    public event PropertyChangedEventHandler? PropertyChanged;
    public event Action? ConflictsChanged;

    private readonly ConcurrentDictionary<string, HashSet<ItemModel>> _registry = new(StringComparer.OrdinalIgnoreCase);

    private int _totalConflicts;
    public int TotalConflicts
    {
        get => _totalConflicts;
        private set
        {
            if (_totalConflicts != value)
            {
                _totalConflicts = value;
                OnPropertyChanged();
            }
        }
    }

    private int _registryVersion;
    public int RegistryVersion
    {
        get => _registryVersion;
        private set
        {
            if (_registryVersion != value)
            {
                _registryVersion = value;
                OnPropertyChanged();
            }
        }
    }

    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }

    public void Register(ItemModel item)
    {
        if (item == null || string.IsNullOrWhiteSpace(item.ItemID)) return;

        var set = _registry.GetOrAdd(item.ItemID, _ => []);
        lock (set)
        {
            int previousCount = set.Count;
            if (set.Add(item))
            {
                if (previousCount == 1)
                {
                    TotalConflicts++;
                    ConflictsChanged?.Invoke();
                    RegistryVersion++;
                }
                else if (previousCount == 0)
                {
                    TotalConflicts++;
                    ConflictsChanged?.Invoke();
                    RegistryVersion++;
                }
                NotifyRegistryChanged();
            }
        }
    }

    public void Unregister(ItemModel item)
    {
        if (item == null || string.IsNullOrWhiteSpace(item.ItemID)) return;

        if (_registry.TryGetValue(item.ItemID, out var set))
        {
            lock (set)
            {
                if (set.Remove(item))
                {
                    TotalConflicts--;
                    ConflictsChanged?.Invoke();
                    RegistryVersion++;

                    if (set.Count == 0)
                        _registry.TryRemove(item.ItemID, out _);
                    NotifyRegistryChanged();
                }
            }
        }
    }

    public void UpdateItemID(ItemModel item, string? oldID, string? newID)
    {
        if (string.Equals(oldID, newID, StringComparison.OrdinalIgnoreCase)) return;

        // Unregister old ID
        if (!string.IsNullOrWhiteSpace(oldID) && _registry.TryGetValue(oldID, out var oldSet))
        {
            lock (oldSet)
            {
                if (oldSet.Remove(item))
                {
                    TotalConflicts--;
                    ConflictsChanged?.Invoke();
                    RegistryVersion++;
                    NotifyRegistryChanged();
                }
            }
        }

        // Register new ID
        if (!string.IsNullOrWhiteSpace(newID))
        {
            var newSet = _registry.GetOrAdd(newID, _ => []);
            lock (newSet)
            {
                int previousCount = newSet.Count;
                if (newSet.Add(item))
                {
                    TotalConflicts++;
                    ConflictsChanged?.Invoke();
                    RegistryVersion++;
                    NotifyRegistryChanged();
                }
            }
        }
    }

    public bool IsIDDuplicate(string? id)
    {
        if (string.IsNullOrWhiteSpace(id)) return false;
        return _registry.TryGetValue(id, out var set) && set.Count > 1;
    }

    private void NotifyRegistryChanged()
    {
        OnPropertyChanged(nameof(TotalConflicts));
    }
}