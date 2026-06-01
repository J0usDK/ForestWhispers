using System.Diagnostics;
using ItemEditor.Core;
using ItemEditor.Models.Schema;
using ItemEditor.Services;

namespace ItemEditor.ViewModels
{
    internal class MainViewModel : ViewModelBase
    {
        private readonly ISchemaService _schemaService;
        private ItemTraitsSchema? _schema;

        public ItemTraitsSchema? Schema
        {
            get => _schema;
            set
            {
                _schema = value;
                OnPropertyChanged();
            }
        }

        public MainViewModel(ISchemaService schemaService)
        {
            _schemaService = schemaService;
            LoadInitialData();
        }

        private void LoadInitialData()
        {
            try
            {
                string testPath = @"C:\Users\Admin\Documents\CRYENGINE Projects\Whispers Project\Data\Schemas\item_traits_schema.json";
                Schema = _schemaService.LoadSchema(testPath);
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"[ERROR] Failed to load schema: {ex.Message}");
            }
        }
    }
}
