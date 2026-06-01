using ItemEditor.Services;
using ItemEditor.ViewModels;
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
            var mainViewModel = new MainViewModel(schemaService);
            var mainWindow = new MainWindow
            {
                DataContext = mainViewModel
            };
            mainWindow.Show();
        }
    }
}
