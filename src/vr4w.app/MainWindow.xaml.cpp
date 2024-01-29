// pch
#include "pch.h"

// codegen
#include "MainWindow.xaml.h"

// winrt deps

// factory
#include "MainWindow.g.cpp"

namespace windowing = winrt::Microsoft::UI::Windowing;

namespace winrt::vr4w_app::implementation {

MainWindow::MainWindow() {
  AppWindow().TitleBar().IconShowOptions(windowing::IconShowOptions::HideIconAndSystemMenu);
}

}  // namespace winrt::vr4w_app::implementation
