using ItemEditor.Services;
using ItemEditor.ViewModels;
using ItemEditor.Views;
using System.Windows;

namespace ItemEditor
{
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            var schemaService = new SchemaService();
            var itemService = new ItemService();
            var settingsService = new SettingsService();
            var dialogService = new DialogService();

            var mainViewModel = new MainViewModel(schemaService, itemService, settingsService, dialogService);
            var mainWindow = new MainWindow
            {
                DataContext = mainViewModel
            };
            mainWindow.Show();
        }
    }
}
