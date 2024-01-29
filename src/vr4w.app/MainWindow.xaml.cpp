// pch
#include "pch.h"

// self header
#include "MainWindow.xaml.h"

// factory
#include "MainWindow.g.cpp"

namespace windowing = winrt::Microsoft::UI::Windowing;

namespace winrt::vr4w_app::implementation {

MainWindow::MainWindow() {
  AppWindow().TitleBar().IconShowOptions(windowing::IconShowOptions::HideIconAndSystemMenu);
}

}  // namespace winrt::vr4w_app::implementation
