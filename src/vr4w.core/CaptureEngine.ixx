module;

// pch
#include "pch.h"

export module vr4w;

import std;

const auto EngineWndClass = TEXT("Vr4wEngineWndClass");

export namespace vr4w {

export class CaptureEngine {
 public:
  CaptureEngine() : engineThrd_([this] { EngineThread(); }) {}
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
