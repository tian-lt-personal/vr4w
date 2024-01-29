// pch
#include "pch.h"

// self header
#include "XResourceString.h"

// factory
#include "XResourceString.g.cpp"

namespace xaml = winrt::Microsoft::UI::Xaml;

namespace winrt::vr4w_app::implementation {

Windows::Foundation::IInspectable XResourceString::ProvideValue() {
  return box_value(
      Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{}.GetString(name_));
}

}  // namespace winrt::vr4w_app::implementation
