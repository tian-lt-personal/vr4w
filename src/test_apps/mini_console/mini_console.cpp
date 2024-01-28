// std headers
#include <format>
#include <iostream>
#include <span>

// windows headers
#include <Windows.h>

#include "CaptureEngine.hpp"

void dumpDevices(std::span<vr4w::DeviceInfo> devices) {
  for (auto& info : devices) {
    auto content =
        std::format(L"Device:\n  DisplayName: {}\n  SymbolicLink: {}\n",
                    info.DisplayName, info.SymbolicLink);
    std::wcout << content;
  }
}

int main() {
  auto devices = vr4w::GetAllDevices();
  dumpDevices(devices);

  vr4w::CaptureEngine engine;
  auto async = [&]() -> vr4w::FireAndForget {
    auto device = co_await engine.CreateDevice(devices[0].SymbolicLink);
    co_await engine.Stop();
  };
  async();
}
