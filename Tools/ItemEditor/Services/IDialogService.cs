namespace ItemEditor.Services;

internal interface IDialogService
{
    string? ShowOpenFileDialog(string title, string filter);
    string? ShowOpenFolderDialog(string title);
    void ShowError(string message, string title = "Error");
    bool ShowConfirm(string message, string title = "Confirm");
}
