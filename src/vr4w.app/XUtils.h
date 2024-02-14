#pragma once

// std headers
#include <functional>

// winrt headers
#include <winrt/Microsoft.UI.Xaml.Input.h>

class RelayCommand
    : public winrt::implements<RelayCommand, winrt::Microsoft::UI::Xaml::Input::ICommand> {
 public:
  template <class F>
  explicit RelayCommand(F&& action) : action_(std::forward<F>(action)) {}
  auto CanExecute(const IInspectable&) const noexcept { return true; }
  void Execute(const IInspectable& param) const { action_(param); }
  auto CanExecuteChanged(
      const winrt::Windows::Foundation::EventHandler<IInspectable>&) const noexcept {
    return winrt::event_token{};
  }
  void CanExecuteChanged(winrt::event_token) const noexcept {}

 private:
  std::function<void(const IInspectable&)> action_;
};
