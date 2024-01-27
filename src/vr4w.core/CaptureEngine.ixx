module;

// pch
#include "pch.h"

export module vr4w;

import std;
import :Awaiters;
import :Coroutines;

const auto EngineWndClass = TEXT("Vr4wEngineWndClass");
constexpr unsigned MaxDeviceIndex =
    9;  // see
        // https://learn.microsoft.com/en-us/windows/win32/api/vfw/nf-vfw-capgetdriverdescriptiona
enum class EngineMessage : unsigned { ResumeOnLoop };

namespace vr4w {

export struct DeviceInfo {
  unsigned Index;
  std::wstring Name;
  std::wstring Version;
};

export std::vector<DeviceInfo> GetAllDevices() {
  std::vector<DeviceInfo> result;
  wchar_t name[MAX_PATH];
  wchar_t version[MAX_PATH];
  for (auto idx = 0u; idx <= MaxDeviceIndex; ++idx) {
    if (capGetDriverDescriptionW(idx, name, MAX_PATH, version, MAX_PATH)) {
      result.push_back(DeviceInfo{.Index = idx, .Name = name, .Version = version});
    }
  }
  return result;
}

export class CaptureEngine {
 public:
  CaptureEngine() : engineThrd_([this] { EngineThread(); }) {}
  details::FireAndForget Connect(unsigned deviceIndex) {
    co_await details::ResumeOnLoop(hwnd_, std::to_underlying(EngineMessage::ResumeOnLoop));
    auto r = SendMessage(hwnd_, WM_CAP_DRIVER_CONNECT, static_cast<WPARAM>(deviceIndex), 0);
    std::terminate();
  }
  void Stop() {}

 private:
  void EngineThread() {
    RegisterWndClass();
    hwnd_ = CreateWindowEx(0, EngineWndClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                           nullptr, this);
    if (!hwnd_) {
      throw std::runtime_error{"xhi7P9DG"};
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
      if (msg.message == std::to_underlying(EngineMessage::ResumeOnLoop)) {
        details::InvokeOnLoop(msg.lParam);
        continue;
      }
      DispatchMessage(&msg);
    }
  }

 private:
  static void RegisterWndClass() {
    WNDCLASSEX wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpszClassName = EngineWndClass;
    wcex.lpfnWndProc = WndProc;
    if (!RegisterClassEx(&wcex)) {
      throw std::runtime_error{"GfUW1lQ4"};
    }
  }

  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
      case WM_CREATE: {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        break;
      }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

 private:
  std::jthread engineThrd_;
  HWND hwnd_;
};

}  // namespace vr4w
