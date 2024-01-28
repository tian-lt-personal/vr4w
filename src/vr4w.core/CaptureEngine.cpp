// pch
#include "pch.h"

// decl
#include "CaptureEngine.hpp"

// references
#include "Awaiters.hpp"
#include "Coroutines.hpp"

namespace {

const auto EngineWndClass = TEXT("Vr4wEngineWndClass");
enum class EngineMessage : unsigned { ResumeOnLoop };

void SafeRelase(auto&& ptr) {
  if (ptr != nullptr) {
    ptr->Release();
  }
}

}  // namespace

namespace vr4w {

struct CaptureEngine::Intl {
  static auto ResumeOnLoop(const CaptureEngine& self) {
    return impl::ResumeOnLoop(self.hwnd_, std::to_underlying(EngineMessage::ResumeOnLoop));
  }
};

std::vector<DeviceInfo> GetAllDevices() noexcept {
  std::vector<DeviceInfo> result;
  wil::com_ptr<IMFAttributes> attrs;
  if (FAILED(MFCreateAttributes(attrs.addressof(), 1))) {
    return result;
  }
  if (FAILED(attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID))) {
    return result;
  }
  UINT32 count;
  IMFActivate** devices;
  if (FAILED(MFEnumDeviceSources(attrs.get(), &devices, &count)) || count == 0) {
    return result;
  }
  auto devicesRevoker = wil::scope_exit([&] {
    for (auto i = 0u; i < count; ++i) {
      SafeRelase(devices[i]);
    }
    CoTaskMemFree(devices);
  });
  for (auto i = 0u; i < count; ++i) {
    wil::unique_cotaskmem_string displayName;
    auto hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                             displayName.addressof(), nullptr);
    if (FAILED(hr)) {
      continue;
    }

    wil::unique_cotaskmem_string symbolicLink;
    hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                        symbolicLink.addressof(), nullptr);
    if (FAILED(hr)) {
      continue;
    }
    result.push_back(
        DeviceInfo{.DisplayName = displayName.get(), .SymbolicLink = symbolicLink.get()});
  }
  return result;
}

Device::Device(Device&& rhs) noexcept : ptr_(std::exchange(rhs.ptr_, nullptr)) {}

Device& Device::operator=(Device&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  ptr_ = std::exchange(rhs.ptr_, nullptr);
  return *this;
}

Device::~Device() {
  if (ptr_ != nullptr) {
    static_cast<IMFMediaSource*>(ptr_)->Release();
  }
}

CaptureEngine::CaptureEngine() : engineThrd_([this] { EngineThread(); }) {
  launchedSignal_.acquire();
}

FireAndForget CaptureEngine::Stop() {
  co_await Intl::ResumeOnLoop(*this);
  PostQuitMessage(0);
}

void CaptureEngine::EngineThread() {
  if (S_OK != CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)) {
    throw std::runtime_error{"nAyB8FtC"};
  }
  RegisterWndClass();
  hwnd_ = CreateWindowEx(0, EngineWndClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr,
                         this);
  if (!hwnd_) {
    throw std::runtime_error{"xhi7P9DG"};
  }
  launchedSignal_.release();
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (msg.message == std::to_underlying(EngineMessage::ResumeOnLoop)) {
      impl::InvokeOnLoop(msg.lParam);
      continue;
    }
    DispatchMessage(&msg);
  }
  CoUninitialize();
}

void CaptureEngine::RegisterWndClass() {
  WNDCLASSEX wcex{};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpszClassName = EngineWndClass;
  wcex.lpfnWndProc = WndProc;
  if (!RegisterClassEx(&wcex)) {
    throw std::runtime_error{"GfUW1lQ4"};
  }
}

LRESULT CALLBACK CaptureEngine::WndProc(HWND hwnd, UINT msg, WPARAM wparam,
                                        LPARAM lparam) noexcept {
  switch (msg) {
    case WM_CREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
      break;
    }
  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}

Task<std::expected<Device, CaptureEnginError>> CaptureEngine::CreateDevice(
    std::wstring symbolicLink) {
  co_await Intl::ResumeOnLoop(*this);
  wil::com_ptr<IMFMediaSource> source;
  wil::com_ptr<IMFAttributes> attrs;
  if (FAILED(MFCreateAttributes(attrs.addressof(), 2))) {
    co_return std::unexpected(CaptureEnginError::MFError);
  }
  if (FAILED(attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID)) ||
      FAILED(attrs->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                              symbolicLink.c_str()))) {
    co_return std::unexpected(CaptureEnginError::MFError);
  }
  if (FAILED(MFCreateDeviceSource(attrs.get(), source.addressof()))) {
    co_return std::unexpected(CaptureEnginError::MFError);
  }
  co_return Device{source.detach()};
}

}  // namespace vr4w
