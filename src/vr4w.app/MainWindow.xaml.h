#pragma once

// codegen
#include "MainWindow.g.h"

namespace winrt::vr4w_app::implementation {

struct MainWindow : MainWindowT<MainWindow> {
  MainWindow();
};

}  // namespace winrt::vr4w_app::implementation

namespace winrt::vr4w_app::factory_implementation {

struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};

}  // namespace winrt::vr4w_app::factory_implementation
