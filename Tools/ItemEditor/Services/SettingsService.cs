using System.Diagnostics;
using System.IO;
using System.Text.Json;
using ItemEditor.Core;

namespace ItemEditor.Services;

internal sealed class SettingsService : ISettingsService
{
    private readonly string _settingsFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "editor_settings.json");
    private readonly JsonSerializerOptions _options = new() { WriteIndented = true };

    private AppSettings? _cachedSettings;

    public AppSettings LoadSettings()
    {
        if (_cachedSettings != null)
            return _cachedSettings;

        if (!File.Exists(_settingsFilePath))
            return new AppSettings();

        try
        {
            using var stream = File.OpenRead(_settingsFilePath);
            _cachedSettings = JsonSerializer.Deserialize<AppSettings>(stream) ?? new AppSettings();
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"[WARNING] Failed to load settings: {ex.Message}");
            _cachedSettings = new AppSettings();
        }
        return _cachedSettings;
    }

    public void SaveSettings(AppSettings settings)
    {
        _cachedSettings = settings;
        try
        {
            using var stream = File.Create(_settingsFilePath);
            JsonSerializer.Serialize(stream, settings, _options);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"[ERROR] Failed to save settings: {ex.Message}");
        }
    }
}
