using System.Windows;
using System.Windows.Documents;

namespace ItemEditor.Views
{
    public partial class CustomMessageBox : Window
    {
        private CustomMessageBox(string message, string title)
        {
            InitializeComponent();
            MessageText.Text = message;
            TitleText.Text = title;
        }

        private void OkButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }

        private void Header_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                DragMove();
        }

        public static void Show(string message, string title, Window? owner = null)
        {
            var msgBox = new CustomMessageBox(message, title);

            if (owner != null)
                msgBox.Owner = owner;
            else if (Application.Current.MainWindow != null && Application.Current.MainWindow.IsVisible)
                msgBox.Owner = Application.Current.MainWindow;

            msgBox.ShowDialog();
        }

        public static bool ShowConfirm(string message, string title, Window? owner = null)
        {
            var msgBox = new CustomMessageBox(message, title);
            msgBox.CancelBtn.Visibility = Visibility.Visible;
            msgBox.CancelBtn.Content = "No";
            msgBox.OkBtn.Content = "Yes";

            if (owner != null)
                msgBox.Owner = owner;
            else if (Application.Current.MainWindow != null && Application.Current.MainWindow.IsVisible)
                msgBox.Owner = Application.Current.MainWindow;

            return msgBox.ShowDialog() == true;
        }
    }
}
