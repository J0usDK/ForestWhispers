using ItemEditor.Core;

namespace ItemEditor.Models.Item
{
    internal class TraitFieldValue : ViewModelBase
    {
        public string Name { get; init; } = string.Empty;
        public string Type { get; init; } = string.Empty;

        public float? Min { get; init; }
        public float? Max { get; init; }

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
