#pragma once

// std
#include <memory>

// vr4w
#include <CaptureEngine.hpp>

// codegen
#include "MainWindow.g.h"

namespace winrt::vr4w_app::implementation {

struct MainWindow : MainWindowT<MainWindow> {
 public:
  MainWindow();

  Microsoft::UI::Xaml::Input::ICommand ToggleRecordCmd();

 private:
  std::unique_ptr<vr4w::CaptureEngine> captureEngine_;
};

}  // namespace winrt::vr4w_app::implementation

namespace winrt::vr4w_app::factory_implementation {

struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};

}  // namespace winrt::vr4w_app::factory_implementation
