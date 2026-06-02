using System.Collections;
using System.ComponentModel;
using ItemEditor.Core;

namespace ItemEditor.Models.Item
{
    internal class TraitFieldValue : ViewModelBase, INotifyDataErrorInfo
    {
        private static readonly float floatMax = 1000000f;
        private static readonly float floatMin = 0.000001f;

        private object? _originalValue;

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
                OnPropertyChanged(nameof(IsDirty));
            }
        }

        public bool IsDirty
        {
            get
            {
                string currentStr = _value?.ToString() ?? string.Empty;
                string originalStr = _originalValue?.ToString() ?? string.Empty;
                return currentStr != originalStr;
            }
        }

        private readonly Dictionary<string, List<string>> _errorsByPropertyName = new();

        public bool HasErrors => _errorsByPropertyName.Any();
        public event EventHandler<DataErrorsChangedEventArgs>? ErrorsChanged;

        public void AcceptChanges()
        {
            _originalValue = _value;
            OnPropertyChanged(nameof(IsDirty));
        }

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
                if (!float.TryParse(strValue,
                    System.Globalization.NumberStyles.Float,
                    System.Globalization.CultureInfo.InvariantCulture,
                    out float floatVal))
                {
                    AddError(nameof(Value), "Must be a valid float number.");
                    return;
                }

                if (Min.HasValue && floatVal < Min.Value)
                    AddError(nameof(Value), $"Value cannot be less than {Min.Value}.");
                if (Max.HasValue && floatVal > Max.Value)
                    AddError(nameof(Value), $"Value cannot be greater than {Max.Value}.");
                if (floatVal != 0.0f && MathF.Abs(floatVal) < floatMin)
                    AddError(nameof(Value), $"Value cannot be less then {floatMin}");
                if (MathF.Abs(floatVal) > floatMax)
                    AddError(nameof(Value), $"Value cannot be greater than {floatMax}");
            }
            else if (Type.ToLower() == "bool" || Type.ToLower() == "boolean")
            {
                if (!bool.TryParse(strValue, out _))
                    AddError(nameof(Value), "Must be 'true' or 'false'.");
            }
        }
    }
}
