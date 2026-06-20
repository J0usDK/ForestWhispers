using CommunityToolkit.Mvvm.ComponentModel;
using ItemEditor.Core.Types;
using ItemEditor.Core.Validation;
using System.Collections;
using System.ComponentModel;
using System.Xml;

namespace ItemEditor.Models.Item;

internal sealed partial class MetadataFieldValue : ObservableObject, INotifyDataErrorInfo
{
    private readonly Dictionary<string, List<string>> _errorsByPropertyName = [];

    public event EventHandler<FieldValueChangedEventArgs>? FieldValueChanged;
    public event EventHandler<DataErrorsChangedEventArgs>? ErrorsChanged;

    public MetadataFieldType FieldType { get; init; } = MetadataFieldType.None;

    [ObservableProperty]
    private string _value = string.Empty;

    public bool HasErrors => _errorsByPropertyName.Count > 0;

    partial void OnValueChanged(string? oldValue, string? newValue)
    {
        ValidateValue();
        FieldValueChanged?.Invoke(this, new FieldValueChangedEventArgs(oldValue, newValue));
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
        var errors = MetadataFieldValidator.Validate(FieldType, Value);

        foreach (var error in errors)
            AddError(nameof(Value), error);
    }
}
