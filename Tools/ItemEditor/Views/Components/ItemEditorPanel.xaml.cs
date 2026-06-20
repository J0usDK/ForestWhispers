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
}
