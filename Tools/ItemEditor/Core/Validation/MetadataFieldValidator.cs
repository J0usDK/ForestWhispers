using ItemEditor.Core.Types;
using System.IO;

namespace ItemEditor.Core.Validation;

internal static class MetadataFieldValidator
{
    private static readonly Dictionary<MetadataFieldType, Func<string, IEnumerable<string>>> _strategies = new()
    {
        { MetadataFieldType.None, ValidateNone },
        { MetadataFieldType.GeometryPath, ValidateGeometryPath },
        { MetadataFieldType.IconPath, ValidateIconPath }
    };

    public static IEnumerable<string> Validate(MetadataFieldType type, string value)
    {
        if (_strategies.TryGetValue(type, out var validationFunc))
            return validationFunc(value);
        return [];
    }

    private static IEnumerable<string> ValidateNone(string value)
    {
        yield break;
    }

    private static IEnumerable<string> ValidateGeometryPath(string value)
    {
        return ValidateFilePath(value, ".cgf");
    }

    private static IEnumerable<string> ValidateIconPath(string value)
    {
        return ValidateFilePath(value, ".dds");
    }

    private static IEnumerable<string> ValidateFilePath(string value, string requiredExtension)
    {
        if (string.IsNullOrWhiteSpace(value))
            yield break;

        if (value.IndexOfAny(Path.GetInvalidPathChars()) > 0)
        {
            yield return "The path format is invalid.";
            yield break;
        }

        string extension = Path.GetExtension(value);
        if (!string.Equals(extension, requiredExtension, StringComparison.OrdinalIgnoreCase))
            yield return $"File must be a '{requiredExtension}' extension.";
    }
}
