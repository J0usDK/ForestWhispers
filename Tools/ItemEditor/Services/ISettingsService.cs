using ItemEditor.Core;

namespace ItemEditor.Services;

internal interface ISettingsService
{
    AppSettings LoadSettings();
    void SaveSettings(AppSettings settings);
}
