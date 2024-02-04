// pch
#include "pch.h"

// self header
#include "MainWindow.xaml.h"

// factory
#include "MainWindow.g.cpp"

namespace windowing = winrt::Microsoft::UI::Windowing;
namespace mui = winrt::Microsoft::UI;

namespace {
winrt::Windows::Graphics::SizeInt32 GetAdaptedSize(int width, int height, HWND hwnd) {
  auto dpi = GetDpiForWindow(hwnd);
  return {MulDiv(width, dpi, 96), MulDiv(height, dpi, 96)};
}

}  // namespace

namespace winrt::vr4w_app::implementation {

MainWindow::MainWindow() {
  auto appWnd = AppWindow();
  appWnd.TitleBar().IconShowOptions(windowing::IconShowOptions::HideIconAndSystemMenu);
  appWnd.ResizeClient(GetAdaptedSize(800, 600, mui::GetWindowFromWindowId(appWnd.Id())));
  captureEngine_ = std::make_unique<vr4w::CaptureEngine>();
  InitializeComponent();
  Closed([this](const winrt::Windows::Foundation::IInspectable&,
                const winrt::Microsoft::UI::Xaml::WindowEventArgs&) { captureEngine_.reset(); });
}

}  // namespace winrt::vr4w_app::implementation
