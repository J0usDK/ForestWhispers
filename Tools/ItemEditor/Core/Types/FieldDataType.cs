namespace ItemEditor.Core.Types;

internal enum FieldDataType
{
    String,
    Float,
    Boolean,
    Unknown
}

internal static class FieldTypeParser
{
    public static FieldDataType Parse(string? type) => type?.ToLowerInvariant() switch
    {
        "float" => FieldDataType.Float,
        "bool" or "boolean" => FieldDataType.Boolean,
        "string" => FieldDataType.String,
        _ => FieldDataType.Unknown
    };
}
