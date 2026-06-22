namespace ItemEditor.Core.Validation.Context;

internal interface IPathValidationContext
{
    string RootDirectory { get; }

    bool IsFileInDirectory(string filePath, string rootPath);
    string? ToRelativePath(string absolutePath);
    bool IsAbsolutePath(string path);
}
