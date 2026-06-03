using System.Collections;
using System.ComponentModel;
using System.Globalization;
using ItemEditor.Core;

namespace ItemEditor.Models.Item;

internal sealed class FieldValueChangedEventArgs(object? oldValue, object? newValue) : EventArgs
{
    public object? OldValue { get; } = oldValue;
    public object? NewValue { get; } = newValue;
}

internal sealed class TraitFieldValue : ViewModelBase, INotifyDataErrorInfo
{
    private static readonly float FloatMax = 1000000f;
    private static readonly float FloatMin = 0.000001f;

    private object? _originalValue;
    private object? _value;
    private readonly Dictionary<string, List<string>> _errorsByPropertyName = [];

    public event EventHandler<FieldValueChangedEventArgs>? FieldValueChanged;
    public event EventHandler<DataErrorsChangedEventArgs>? ErrorsChanged;

    public string Name { get; init; } = string.Empty;
    public string Type { get; init; } = string.Empty;
    public float? Min { get; init; }
    public float? Max { get; init; }

    public object? Value
    {
        get => _value;
        set
        {
            if (Equals(_value, value)) return;

            object? oldValue = _value;
            _value = value;

            OnPropertyChanged();
            ValidateValue();
            OnPropertyChanged(nameof(IsDirty));

            FieldValueChanged?.Invoke(this, new FieldValueChangedEventArgs(oldValue, value));
        }
    }

    public bool IsDirty => !Equals(_value, _originalValue);
    public bool HasErrors => _errorsByPropertyName.Count > 0;

    public TraitFieldValue Clone()
    {
        return new TraitFieldValue
        {
            Name = this.Name,
            Type = this.Type,
            Min = this.Min,
            Max = this.Max,
            Value = this.Value
        };
    }

    public void AcceptChanges()
    {
        _originalValue = _value;
        OnPropertyChanged(nameof(IsDirty));
    }

    public IEnumerable GetErrors(string? propertyName)
    {
        if (string.IsNullOrEmpty(propertyName) || !_errorsByPropertyName.TryGetValue(propertyName, out var errors))
            return Enumerable.Empty<string>();
        return errors;
    }

    private void AddError(string propertyName, string error)
    {
        if (!_errorsByPropertyName.TryGetValue(propertyName, out var errors))
        {
            errors = [];
            _errorsByPropertyName[propertyName] = errors;
        }

        if (!errors.Contains(error))
        {
            errors.Add(error);
            ErrorsChanged?.Invoke(this, new DataErrorsChangedEventArgs(propertyName));
        }
    }

    private void ClearErrors(string propertyName)
    {
        if (_errorsByPropertyName.Remove(propertyName))
            ErrorsChanged?.Invoke(this, new DataErrorsChangedEventArgs(propertyName));
    }

    private void ValidateValue()
    {
        ClearErrors(nameof(Value));
        string strValue = Value?.ToString() ?? string.Empty;
        var dataType = FieldTypeParser.Parse(Type);
        
        switch (dataType)
        {
            case FieldDataType.Float:
                if (!float.TryParse(strValue, NumberStyles.Float, CultureInfo.InvariantCulture, out var floatVal))
                {
                    AddError(nameof(Value), "Must be a valid float number.");
                    return;
                }
                if (Min.HasValue && floatVal < Min.Value)
                    AddError(nameof(Value), $"Value cannot be less than {Min.Value}.");
                if (Max.HasValue && floatVal > Max.Value)
                    AddError(nameof(Value), $"Value cannot be greater than {Max.Value}.");
                if (floatVal != 0.0f && MathF.Abs(floatVal) < FloatMin)
                    AddError(nameof(Value), $"Value cannot be less then {FloatMin}");
                if (MathF.Abs(floatVal) > FloatMax)
                    AddError(nameof(Value), $"Value cannot be greater than {FloatMax}");
                break;

            case FieldDataType.Boolean:
                if (!bool.TryParse(strValue, out _))
                    AddError(nameof(Value), "Must be 'true' or 'false'.");
                break;

            case FieldDataType.String:
            case FieldDataType.Unknown:
            default:
                break;
        }
    }
}
