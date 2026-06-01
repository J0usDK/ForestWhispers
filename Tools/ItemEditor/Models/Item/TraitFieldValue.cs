using ItemEditor.Core;

namespace ItemEditor.Models.Item
{
    internal class TraitFieldValue : ViewModelBase
    {
        public string Name { get; init; } = string.Empty;

        private object _value = string.Empty;
        public object Value
        {
            get => _value;
            set
            {
                _value = value;
                OnPropertyChanged();
            }
        }
    }
}
