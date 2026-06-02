using System.Windows;
using System.Windows.Interop;

namespace ItemEditor.Views
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);

            IntPtr hwnd = new WindowInteropHelper(this).Handle;

            HwndSource.FromHwnd(hwnd)?.AddHook(WindowProc);
        }

        private IntPtr WindowProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            const int WM_NCRBUTTONDOWN = 0x00A4;
            const int WM_NCRBUTTONUP = 0x00A5;
            const int HTCAPTION = 2;

            if ((msg == WM_NCRBUTTONUP || msg == WM_NCRBUTTONDOWN) && wParam.ToInt32() == HTCAPTION)
            {
                handled = true;
                return IntPtr.Zero;
            }
            return IntPtr.Zero;
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            base.OnClosing(e);
            if (DataContext == null)
                return;

            dynamic viewModel = DataContext;
            bool hasUnsavedChanges = false;

            try
            {
                foreach (dynamic item in viewModel.LoadedItems)
                {
                    if (item.IsDirty)
                    {
                        hasUnsavedChanges = true;
                        break;
                    }
                }
            }
            catch { }

            if (hasUnsavedChanges)
            {
                bool confirmExit = CustomMessageBox.ShowConfirm(
                    "You have unsaved changes!\nAre you sure you want to exit and lose all unsaved progress?",
                    "Unsaved Changes", this);

                if (!confirmExit)
                    e.Cancel = true;
            }
        }

        private void MinimizeWindow_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        private void MaximizeWindow_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;
        }

        private void CloseWindow_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}