#pragma once

// std headers
#include <expected>
#include <memory>
#include <semaphore>
#include <string>
#include <thread>
#include <vector>

// directx headers
#include <d3d11_4.h>

// windows headers
#include <windef.h>

// wil headers
#include <wil/com.h>

// vr4w headers
#include "Coroutines.hpp"

// about MF:
// https://learn.microsoft.com/en-us/windows/win32/medfound/media-foundation-programming--essential-concepts

namespace {

struct PtrAccessor;

}  // namespace

namespace vr4w {

enum class CaptureEngineError { Unknown, MFError };

struct DeviceInfo {
  std::wstring DisplayName;
  std::wstring SymbolicLink;
};

// opaque types
struct Device;
struct RecordingContext;

std::vector<DeviceInfo> GetAllDevices() noexcept;

class CaptureEngine {
  struct Intl;

 public:
  CaptureEngine();
  ~CaptureEngine();
  Task<std::expected<std::shared_ptr<Device>, CaptureEngineError>> CreateDevice(
      std::wstring symbolicLink) const noexcept;
  Task<std::expected<std::shared_ptr<RecordingContext>, CaptureEngineError>> Start(
      std::shared_ptr<Device> device, unsigned width, unsigned height) const noexcept;

 private:
  void EngineThread();

 private:
  static void RegisterWndClass();
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

 private:
  std::binary_semaphore launchedSignal_{0};
  std::jthread engineThrd_;
  HWND hwnd_ = nullptr;
  DWORD tid_ = 0;

  wil::com_ptr<ID3D11Device> d3ddev_;
  wil::com_ptr<ID3D11DeviceContext> d3ddevctx_;
};

}  // namespace vr4w
