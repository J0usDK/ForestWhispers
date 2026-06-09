namespace ItemEditor.Core;

internal sealed record AppSettings
{
    private readonly string _lastSchemaPath = string.Empty;
    public string LastSchemaPath
    {
        get => _lastSchemaPath;
        init => _lastSchemaPath = value ?? string.Empty;
    }

    private readonly string _lastItemsDirectory = string.Empty;
    public string LastItemsDirectory
    {
        get => _lastItemsDirectory;
        init => _lastItemsDirectory = value ?? string.Empty;
    }
}
