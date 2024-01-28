#pragma once

// std headers
#include <string>
#include <thread>
#include <vector>

// windows headers
#include <windef.h>

// vr4w headers
#include "Coroutines.hpp"

// about MF:
// https://learn.microsoft.com/en-us/windows/win32/medfound/media-foundation-programming--essential-concepts

namespace vr4w {

struct DeviceInfo {
  std::wstring DisplayName;
  std::wstring SymbolicLink;
};

std::vector<DeviceInfo> GetAllDevices() noexcept;

class CaptureEngine {
 public:
  CaptureEngine();
  impl::FireAndForget Connect(unsigned deviceIndex);
  void Stop();

 private:
  void EngineThread();

 private:
  static void RegisterWndClass();
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

 private:
  std::jthread engineThrd_;
  HWND hwnd_;
};

}  // namespace vr4w
