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

std::expected<wil::com_ptr<IMFAttributes>, vr4w::CaptureEngineError> CreateEmptyAttributes() {
  wil::com_ptr<IMFAttributes> attrs;
  if (FAILED(MFCreateAttributes(attrs.addressof(), 0))) {
    return std::unexpected(vr4w::CaptureEngineError::MFError);
  }
  return attrs;
}

struct Singularity {
  Singularity() = default;
  Singularity(const Singularity&) = delete;
  Singularity(Singularity&&) = delete;
  Singularity& operator=(const Singularity&) = delete;
  Singularity& operator=(Singularity&&) = delete;
};

}  // namespace

namespace vr4w {

// claim opaque types
struct Device : Singularity {
  constexpr explicit Device(IMFMediaSource* ptr) noexcept : Ptr(ptr) {}
  ~Device() {
    Ptr->Shutdown();
    Ptr->Release();
  }

  IMFMediaSource* Ptr;
};

struct RecordingContext : Singularity {
  explicit RecordingContext(wil::com_ptr<IMFSourceReader> sourceReader) noexcept
      : SourceReader(std::move(sourceReader)) {}

  wil::com_ptr<IMFSourceReader> SourceReader;
};

// global functions
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

// internal CaptureEngine
struct CaptureEngine::Intl {
  static auto ResumeOnLoop(const CaptureEngine& self) {
    return impl::ResumeOnLoop(self.hwnd_, std::to_underlying(EngineMessage::ResumeOnLoop));
  }
  static bool NotInApartment(const CaptureEngine& self) {
    return GetCurrentThreadId() != self.tid_;
  }
};

CaptureEngine::CaptureEngine() : engineThrd_([this] { EngineThread(); }) {
  launchedSignal_.acquire();
}

Task<> CaptureEngine::Stop() {
  if (Intl::NotInApartment(*this)) {
    co_await Intl::ResumeOnLoop(*this);
  }
  PostQuitMessage(0);
}

void CaptureEngine::EngineThread() {
  SetThreadDescription(GetCurrentThread(), L"vr4w.core.capture_engine");
  if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
    throw std::runtime_error{"nAyB8FtC"};
  }
  if (FAILED(MFStartup(MF_VERSION, MFSTARTUP_LITE))) {
    throw std::runtime_error{"mkPCPu1S"};
  }
  RegisterWndClass();
  hwnd_ = CreateWindowEx(0, EngineWndClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr,
                         this);
  if (!hwnd_) {
    throw std::runtime_error{"xhi7P9DG"};
  }
  tid_ = GetCurrentThreadId();
  launchedSignal_.release();
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (msg.message == std::to_underlying(EngineMessage::ResumeOnLoop)) {
      impl::InvokeOnLoop(msg.lParam);
      continue;
    }
    DispatchMessage(&msg);
  }
  MFShutdown();
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
      return 0;
    }
  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}

Task<std::expected<std::shared_ptr<Device>, CaptureEngineError>> CaptureEngine::CreateDevice(
    std::wstring symbolicLink) const noexcept {
  if (Intl::NotInApartment(*this)) {
    co_await Intl::ResumeOnLoop(*this);
  }
  wil::com_ptr<IMFMediaSource> source;
  wil::com_ptr<IMFAttributes> attrs;
  if (FAILED(MFCreateAttributes(attrs.addressof(), 2))) {
    co_return std::unexpected(CaptureEngineError::MFError);
  }
  if (FAILED(attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID)) ||
      FAILED(attrs->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                              symbolicLink.c_str()))) {
    co_return std::unexpected(CaptureEngineError::MFError);
  }
  if (FAILED(MFCreateDeviceSource(attrs.get(), source.addressof()))) {
    co_return std::unexpected(CaptureEngineError::MFError);
  }
  co_return std::make_shared<Device>(source.detach());
}

Task<std::expected<std::shared_ptr<RecordingContext>, CaptureEngineError>> CaptureEngine::Start(
    std::shared_ptr<Device> device) const noexcept {
  if (Intl::NotInApartment(*this)) {
    co_await Intl::ResumeOnLoop(*this);
  }
  auto emptyAttrs = CreateEmptyAttributes();
  if (!emptyAttrs.has_value()) {
    co_return std::unexpected(emptyAttrs.error());
  }
  wil::com_ptr<IMFSourceReader> reader;
  if (FAILED(MFCreateSourceReaderFromMediaSource(device->Ptr, emptyAttrs->get(),
                                                 reader.addressof()))) {
    co_return std::unexpected(CaptureEngineError::MFError);
  }
  co_return std::make_shared<RecordingContext>(reader.detach());
}

}  // namespace vr4w
