using ItemEditor.Core.Validation.Context;
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
            var settingsService = new SettingsService();
            var dialogService = new DialogService();
            var itemIDRegistryService = new ItemIDRegistryService();
            var enginePathService = new EnginePathService(dialogService);
            var pathValidationContext = new PathValidationContext(settingsService, enginePathService);
            var itemService = new ItemService(pathValidationContext);

            var itemsListViewModel = new ItemsListViewModel(itemService, dialogService, settingsService, itemIDRegistryService);
            var itemEditorViewModel = new ItemEditorViewModel(itemService, settingsService, dialogService, itemIDRegistryService, enginePathService);
            var traitsListViewModel = new TraitsListViewModel();

            var mainViewModel = new MainViewModel(schemaService, itemService, settingsService, dialogService, itemIDRegistryService, itemsListViewModel, itemEditorViewModel, traitsListViewModel);
            var mainWindow = new MainWindow
            {
                DataContext = mainViewModel
            };
            mainWindow.Show();
        }
    }
}
