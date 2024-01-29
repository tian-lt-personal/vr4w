#pragma once

// codegen
#include "XResourceString.g.h"

namespace winrt::vr4w_app::implementation {

class XResourceString : public XResourceStringT<XResourceString> {
 public:
  hstring Name() const { return name_; }
  void Name(const hstring& value) { name_ = value; }

  IInspectable ProvideValue(winrt::Microsoft::UI::Xaml::IXamlServiceProvider) {
    return ProvideValue();
  }
  IInspectable ProvideValue();

 private:
  hstring name_;
};

}  // namespace winrt::vr4w_app::implementation

namespace winrt::vr4w_app::factory_implementation {

struct XResourceString : XResourceStringT<XResourceString, implementation::XResourceString> {};

}  // namespace winrt::vr4w_app::factory_implementation
