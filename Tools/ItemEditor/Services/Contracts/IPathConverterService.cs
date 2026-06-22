namespace ItemEditor.Services.Contracts;

internal interface IPathConverterService
{
    string? ToRelativePath(string absolutePath, string rootDirection);
    bool IsAbsolutePath(string path);
}
