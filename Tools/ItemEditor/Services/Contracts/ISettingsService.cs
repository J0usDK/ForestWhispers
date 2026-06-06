using ItemEditor.Core;

namespace ItemEditor.Services.Contracts;

internal interface ISettingsService
{
    AppSettings LoadSettings();
    void SaveSettings(AppSettings settings);
}
