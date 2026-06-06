using Microsoft.Win32;
using ItemEditor.Views;
using ItemEditor.Services.Contracts;

namespace ItemEditor.Services;

internal sealed class DialogService : IDialogService
{
    public string? ShowOpenFileDialog(string title, string filter)
    {
        var dialog = new OpenFileDialog
        {
            Title = title,
            Filter = filter
        };

        return dialog.ShowDialog() == true ? dialog.FileName : null;
    }

    public string? ShowOpenFolderDialog(string title)
    {
        var dialog = new OpenFolderDialog
        {
            Title = title
        };

        return dialog.ShowDialog() == true ? dialog.FolderName : null;
    }

    public void ShowError(string message, string title = "Error")
    {
        CustomMessageBox.Show(message, title);
    }

    public bool ShowConfirm(string message, string title = "Confirm")
    {
        return CustomMessageBox.ShowConfirm(message, title);
    }
}
