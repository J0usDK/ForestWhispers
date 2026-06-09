using System.Collections;
using System.ComponentModel;
using CommunityToolkit.Mvvm.ComponentModel;
using ItemEditor.Core.Types;
using ItemEditor.Core.Validation;

namespace ItemEditor.Models.Item;

internal sealed class FieldValueChangedEventArgs(object? oldValue, object? newValue) : EventArgs
{
    public object? OldValue { get; } = oldValue;
    public object? NewValue { get; } = newValue;
}

internal sealed partial class TraitFieldValue : ObservableObject, INotifyDataErrorInfo
{
    private object? _originalValue;
    private readonly Dictionary<string, List<string>> _errorsByPropertyName = [];

    public event EventHandler<FieldValueChangedEventArgs>? FieldValueChanged;
    public event EventHandler<DataErrorsChangedEventArgs>? ErrorsChanged;

    public string Name { get; init; } = string.Empty;
    public FieldDataType DataType { get; private set; } = FieldDataType.Unknown;
    private readonly string _type = string.Empty;
    public string Type
    {
        get => _type;
        init
        {
            _type = value;
            DataType = FieldTypeParser.Parse(value);
        }
    }
    public float? Min { get; init; }
    public float? Max { get; init; }

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(IsDirty))]
    private object? _value;

    public bool IsDirty => !Equals(Value, _originalValue);
    public bool HasErrors => _errorsByPropertyName.Count > 0;

    partial void OnValueChanged(object? oldValue, object? newValue)
    {
        ValidateValue();
        FieldValueChanged?.Invoke(this, new FieldValueChangedEventArgs(oldValue, newValue));
    }

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
        _originalValue = Value;
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
            OnPropertyChanged(nameof(HasErrors));
        }
    }

    private void ClearErrors(string propertyName)
    {
        if (_errorsByPropertyName.Remove(propertyName))
        {
            ErrorsChanged?.Invoke(this, new DataErrorsChangedEventArgs(propertyName));
            OnPropertyChanged(nameof(HasErrors));
        }
    }

    private void ValidateValue()
    {
        ClearErrors(nameof(Value));
        string strValue = Value?.ToString() ?? string.Empty;

        var errors = FieldValidator.Validate(DataType, strValue, Min, Max);

        foreach (var error in errors)
            AddError(nameof(Value), error);
    }
}
