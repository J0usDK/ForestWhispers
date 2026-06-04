using ItemEditor.ViewModels;
using System.ComponentModel;
using System.Windows;
using System.Windows.Interop;

namespace ItemEditor.Views;

public partial class MainWindow : Window
{
    private HwndSource? _hwndSource;

    public MainWindow()
    {
        InitializeComponent();
    }

    protected override void OnSourceInitialized(EventArgs e)
    {
        base.OnSourceInitialized(e);
        IntPtr hwnd = new WindowInteropHelper(this).Handle;
        _hwndSource = HwndSource.FromHwnd(hwnd);
        _hwndSource?.AddHook(WindowProc);
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

    protected override void OnClosing(CancelEventArgs e)
    {
        base.OnClosing(e);
        
        if (DataContext is MainViewModel viewModel)
        {
            if (!viewModel.RequestApplicationClose())
                e.Cancel = true;
        }
    }

    protected override void OnClosed(EventArgs e)
    {
        _hwndSource?.RemoveHook(WindowProc);
        _hwndSource = null;
        base.OnClosed(e);
    }

    private void MinimizeWindow_Click(object sender, RoutedEventArgs e) =>
        WindowState = WindowState.Minimized;

    private void MaximizeWindow_Click(object sender, RoutedEventArgs e) =>
        WindowState = WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;

    private void CloseWindow_Click(object sender, RoutedEventArgs e) =>
        Close();
}