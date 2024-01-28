#pragma once

// std headers
#include <expected>
#include <semaphore>
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

enum class CaptureEnginError { Unknown, MFError };

struct DeviceInfo {
  std::wstring DisplayName;
  std::wstring SymbolicLink;
};

class Device {
  friend std::expected<Device, CaptureEnginError> CreateDevice(std::wstring symbolicLink) noexcept;

 public:
  Device(const Device&) = delete;
  Device(Device&&) noexcept;

  Device& operator=(const Device&) = delete;
  Device& operator=(Device&&) noexcept;
  ~Device();

 private:
  explicit Device(void* ptr) noexcept : ptr_(ptr) {}

 private:
  void* ptr_;
};

std::vector<DeviceInfo> GetAllDevices() noexcept;
std::expected<Device, CaptureEnginError> CreateDevice(std::wstring symbolicLink) noexcept;

class CaptureEngine {
 public:
  CaptureEngine();
  impl::FireAndForget Connect(unsigned deviceIndex);
  impl::FireAndForget Stop();

 private:
  void EngineThread();

 private:
  static void RegisterWndClass();
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

 private:
  std::binary_semaphore launchedSignal_{0};
  std::jthread engineThrd_;
  HWND hwnd_ = nullptr;
};

}  // namespace vr4w
