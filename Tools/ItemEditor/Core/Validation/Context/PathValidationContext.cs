using ItemEditor.Services.Contracts;

namespace ItemEditor.Core.Validation.Context;

internal sealed class PathValidationContext(ISettingsService settingsService, IEnginePathService enginePathService, IPathConverterService pathConverter) : IPathValidationContext
{
    private readonly ISettingsService _settingsService = settingsService;
    private readonly IEnginePathService _enginePathService = enginePathService;
    private readonly IPathConverterService _pathConverter = pathConverter;

    public string RootDirectory => _settingsService.LoadSettings().LastProjectDirectory;

    public bool IsFileInDirectory(string filePath, string rootPath) => _enginePathService.IsFileInDirectory(filePath, rootPath);

    public string? ToRelativePath(string absolutePath) => _pathConverter.ToRelativePath(absolutePath, RootDirectory);

    public bool IsAbsolutePath(string path) => _pathConverter.IsAbsolutePath(path);
}
