using System.Globalization;
using ItemEditor.Core.Types;

namespace ItemEditor.Core.Validation;

internal static class FieldValidator
{
    private static readonly float FloatMax = 1000000f;
    private static readonly float FloatMin = 0.000001f;

    private static readonly Dictionary<FieldDataType, Func<string, float?, float?, IEnumerable<string>>> _strategies = new()
    {
        {FieldDataType.Float, ValidateFloat },
        {FieldDataType.Boolean, ValidateBool },
        {FieldDataType.String, ValidateString }
    };

    public static IEnumerable<string> Validate(FieldDataType type, string? stringValue, float? min, float? max)
    {
        if (_strategies.TryGetValue(type, out var validationFunc))
            return validationFunc(stringValue ?? string.Empty, min, max);
        return [];
    }

    private static IEnumerable<string> ValidateFloat(string value, float? min, float? max)
    {
        if (!float.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out float floatVal))
        {
            yield return "Must be a valid float number.";
            yield break;
        }

        if (min.HasValue && floatVal < min.Value)
            yield return $"Value cannot be less than {min.Value}";
        if (max.HasValue && floatVal > max.Value)
            yield return $"Value cannot be greater than {max.Value}.";

        if (floatVal != 0.0f && MathF.Abs(floatVal) < FloatMin)
            yield return $"Value cannot be less than {FloatMin}";
        if (MathF.Abs(floatVal) > FloatMax)
            yield return $"Value cannot be greater than {FloatMax}";
    }

    private static IEnumerable<string> ValidateBool(string value, float? min, float? max)
    {
        if (!bool.TryParse(value, out _))
            yield return "Must be 'true' or 'false'.";
    }

    private static IEnumerable<string> ValidateString(string value, float? min, float? max)
    {
        yield break;
    }
}
