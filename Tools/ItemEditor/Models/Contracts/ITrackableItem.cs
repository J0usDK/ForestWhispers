using System.ComponentModel;

namespace ItemEditor.Models.Contracts;

internal interface ITrackableItem : INotifyPropertyChanged
{
    bool IsDirty { get; }
    bool HasErrors { get; }
    bool IsIDDuplicate { get; }

    void AcceptChanges();
}
