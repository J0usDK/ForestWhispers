using ItemEditor.ViewModels;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ItemEditor.Views.Components;

public partial class ItemEditorPanel : UserControl
{
    public ItemEditorPanel()
    {
        InitializeComponent();
    }

    private void OnTextBoxTextChanged(object sender, TextChangedEventArgs args)
    {
        if (sender is TextBox textBox && textBox.IsFocused && args.Changes.Any(c => c.Offset == 0 && c.AddedLength == textBox.Text.Length))
            textBox.CaretIndex = textBox.Text.Length;
    }

    private void PathTextBox_PreviewKeyDown(object sender, KeyEventArgs e)
    {
        if (e.Key == Key.Return && sender is TextBox textBox)
        {
            var binding = textBox.GetBindingExpression(TextBox.TextProperty);
            binding?.UpdateSource();
        }
    }
    private void GeometryPathTextBox_LostFocus(object sender, RoutedEventArgs e)
    {
        if (DataContext is MainViewModel vm)
            vm.ItemEditor.ConvertGeometryPathCommand.Execute(null);
    }

    private void IconPathTextBox_LostFocus(object sender, RoutedEventArgs e)
    {
        if (DataContext is MainViewModel vm)
            vm.ItemEditor.ConvertIconPathCommand.Execute(null);
    }
}
