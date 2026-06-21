using ItemEditor.Services.Contracts;
using System.IO;

namespace ItemEditor.Services;

internal sealed class EnginePathService(IDialogService dialogService) : IEnginePathService
{
    private readonly IDialogService _dialogService = dialogService;

    public string ConvertToEnginePath(string absolutePath, string rootPath)
    {
        if (string.IsNullOrWhiteSpace(absolutePath))
            return String.Empty;

        if (string.IsNullOrWhiteSpace(rootPath))
        {
            _dialogService.ShowError("The project root directory should be selected.");
            return String.Empty;
        }

        if (!IsFileInDirectory(absolutePath, rootPath))
        {
            _dialogService.ShowError("The chosen file is not in the selected root directory.");
            return String.Empty;
        }

        string relativePath = Path.GetRelativePath(rootPath, absolutePath);
        return relativePath.Replace('\\', '/');
    }

    public bool IsFileInDirectory(string absolutePath, string rootPath)
    {
        string fullFilePath = Path.GetFullPath(absolutePath);
        string fullDirectoryPath = Path.GetFullPath(rootPath);

        fullDirectoryPath = fullDirectoryPath.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar) + Path.DirectorySeparatorChar;
        return fullFilePath.StartsWith(fullDirectoryPath, StringComparison.OrdinalIgnoreCase);
    }
}
