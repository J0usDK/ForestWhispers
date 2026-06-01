using ItemEditor.Services;
using ItemEditor.ViewModels;
using ItemEditor.Views;
using System.Windows;

namespace ItemEditor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            var schemaService = new SchemaService();
            var itemService = new ItemService();

            var mainViewModel = new MainViewModel(schemaService, itemService);
            var mainWindow = new MainWindow
            {
                DataContext = mainViewModel
            };
            mainWindow.Show();
        }
    }
}
