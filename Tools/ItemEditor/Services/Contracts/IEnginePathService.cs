namespace ItemEditor.Services.Contracts;

internal interface IEnginePathService
{
    string ConvertToEnginePath(string absolutePath, string rootPath);
    bool IsFileInDirectory(string filePath, string rootPath);
}
