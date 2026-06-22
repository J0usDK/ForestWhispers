using ItemEditor.Services.Contracts;
using System.IO;

namespace ItemEditor.Services;

internal sealed class PathConverterService : IPathConverterService
{
    public string? ToRelativePath(string absolutePath, string rootDirectory)
    {
        if (string.IsNullOrWhiteSpace(absolutePath) || string.IsNullOrWhiteSpace(rootDirectory))
            return null;

        string fullFilePath = Path.GetFullPath(absolutePath);
        string fullRootPath = Path.GetFullPath(rootDirectory);

        if (!fullFilePath.StartsWith(fullRootPath, StringComparison.OrdinalIgnoreCase))
            return null;

        string relativePath = Path.GetRelativePath(rootDirectory, absolutePath);
        return relativePath.Replace('\\', '/');
    }

    public bool IsAbsolutePath(string path) => !string.IsNullOrWhiteSpace(path) && Path.IsPathRooted(path);
}
