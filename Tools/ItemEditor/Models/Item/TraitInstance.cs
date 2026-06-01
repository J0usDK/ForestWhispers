using System.Collections.ObjectModel;
using ItemEditor.Core;

namespace ItemEditor.Models.Item
{
    internal class TraitInstance : ViewModelBase
    {
        public string Id { get; init; } = string.Empty;
        public ObservableCollection<TraitFieldValue> Fields { get; } = new();
    }
}
