namespace ItemEditor.Core.Validation.Context;

internal interface IPathValidationContext
{
    string RootDirectory { get; }

    bool IsFileInDirectory(string filePath, string rootPath);
}
