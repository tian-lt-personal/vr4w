// pch
#include "pch.h"

// codegen
#include "App.xaml.h"

// dependencies
#include "MainWindow.xaml.h"

namespace xaml = winrt::Microsoft::UI::Xaml;

namespace winrt::vr4w_app::implementation {

App::App() {
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException([](IInspectable const&, xaml::UnhandledExceptionEventArgs const& e) {
    if (IsDebuggerPresent()) {
      auto errorMessage = e.Message();
      __debugbreak();
    }
  });
#endif
}

void App::OnLaunched([[maybe_unused]] xaml::LaunchActivatedEventArgs const& e) {
  window = winrt::make<MainWindow>();
  window.Activate();
}

}  // namespace winrt::vr4w_app::implementation
