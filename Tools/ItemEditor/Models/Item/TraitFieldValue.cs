using System.Collections;
using System.ComponentModel;
using ItemEditor.Core;

namespace ItemEditor.Models.Item
{
    internal class TraitFieldValue : ViewModelBase, INotifyDataErrorInfo
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
                ValidateValue();
            }
        }

        private readonly Dictionary<string, List<string>> _errorsByPropertyName = new();

        public bool HasErrors => _errorsByPropertyName.Any();
        public event EventHandler<DataErrorsChangedEventArgs>? ErrorsChanged;

        public IEnumerable GetErrors(string? propertyName)
        {
            if (string.IsNullOrEmpty(propertyName) || !_errorsByPropertyName.ContainsKey(propertyName))
                return Enumerable.Empty<string>();
            return _errorsByPropertyName[propertyName];
        }

        private void AddError(string propertyName, string error)
        {
            if (!_errorsByPropertyName.ContainsKey(propertyName))
                _errorsByPropertyName[propertyName] = new List<string>();

            if (!_errorsByPropertyName[propertyName].Contains(error))
            {
                _errorsByPropertyName[propertyName].Add(error);
                ErrorsChanged?.Invoke(this, new DataErrorsChangedEventArgs(propertyName));
            }
        }

        private void ClearErrors(string propertyName)
        {
            if (_errorsByPropertyName.ContainsKey(propertyName))
            {
                _errorsByPropertyName.Remove(propertyName);
                ErrorsChanged?.Invoke(this, new DataErrorsChangedEventArgs(propertyName));
            }
        }

        private void ValidateValue()
        {
            ClearErrors(nameof(Value));
            string strValue = Value?.ToString() ?? string.Empty;

            if (Type.ToLower() == "float")
            {
                if (!float.TryParse(strValue, out float floatVal))
                {
                    AddError(nameof(Value), "Must be a valid float number.");
                    return;
                }

                if (Min.HasValue && floatVal < Min.Value)
                    AddError(nameof(Value), $"Value cannot be less than {Min.Value}.");
                if (Max.HasValue && floatVal > Max.Value)
                    AddError(nameof(Value), $"Value cannot be greater than {Max.Value}.");
            }
            else if (Type.ToLower() == "bool" || Type.ToLower() == "boolean")
            {
                if (!bool.TryParse(strValue, out _))
                    AddError(nameof(Value), "Must be 'true' or 'false'.");
            }
        }
    }
}
