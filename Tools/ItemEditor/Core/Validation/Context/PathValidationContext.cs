using ItemEditor.Services.Contracts;

namespace ItemEditor.Core.Validation.Context;

internal sealed class PathValidationContext(ISettingsService settingsService, IEnginePathService enginePathService) : IPathValidationContext
{
    private readonly ISettingsService _settingsService = settingsService;
    private readonly IEnginePathService _enginePathService = enginePathService;

    public string RootDirectory => _settingsService.LoadSettings().LastProjectDirectory;

    public bool IsFileInDirectory(string filePath, string rootPath) => _enginePathService.IsFileInDirectory(filePath, rootPath);
}
