using ItemEditor.Core.Types;
using ItemEditor.Core.Validation.Context;
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

    public static IEnumerable<string> Validate(MetadataFieldType type, string value, IPathValidationContext? context)
    {
        var syntacticErrors = Validate(type, value);
        foreach (var error in syntacticErrors)
            yield return error;

        if (syntacticErrors.Any())
            yield break;

        if (context != null && type is MetadataFieldType.GeometryPath or MetadataFieldType.IconPath)
        {
            foreach (var error in ValidatePathContext(value, context))
                yield return error;
        }
    }

    private static IEnumerable<string> ValidatePathContext(string value, IPathValidationContext context)
    {
        if (string.IsNullOrEmpty(value))
            yield break;

        string rootDir = context.RootDirectory;
        if (string.IsNullOrWhiteSpace(rootDir))
        {
            yield return "Project root directory is not selected.";
            yield break;
        }

        string absolutePath = Path.Combine(rootDir, value.Replace('/', '\\'));
        if (!File.Exists(absolutePath))
        {
            yield return "File does not exist.";
            yield break;
        }

        if (!context.IsFileInDirectory(absolutePath, rootDir))
            yield return "File must be inside the project Assets directory.";
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
        return ValidateFilePath(value, ".tif");
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
