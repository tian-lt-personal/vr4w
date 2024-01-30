#pragma once

#include <CaptureEngine.hpp>

// codegen
#include "MainWindow.g.h"

namespace winrt::vr4w_app::implementation {

struct MainWindow : MainWindowT<MainWindow> {
  MainWindow();

 private:
  vr4w::CaptureEngine captureEngine_;
};

}  // namespace winrt::vr4w_app::implementation

namespace winrt::vr4w_app::factory_implementation {

struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};

}  // namespace winrt::vr4w_app::factory_implementation
