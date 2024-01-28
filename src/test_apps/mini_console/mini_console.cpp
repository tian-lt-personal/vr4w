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
  auto cap = vr4w::CreateDevice(devices[0].SymbolicLink);
  engine.Stop();
}
