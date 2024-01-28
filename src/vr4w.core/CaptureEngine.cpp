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

template <class P>
void SafeRelase(P ptr) {
  if (ptr != nullptr) {
    ptr->Release();
  }
}

}  // namespace

namespace vr4w {

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
  });
  for (auto i = 0u; i < count; ++i) {
    wil::unique_cotaskmem_string displayName;
    auto hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                             displayName.addressof(), nullptr);
    hr;
  }
  return result;
}

CaptureEngine::CaptureEngine() : engineThrd_([this] { EngineThread(); }) {}
impl::FireAndForget CaptureEngine::Connect(unsigned deviceIndex) { co_return; }
void CaptureEngine::Stop() {}

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

}  // namespace vr4w
