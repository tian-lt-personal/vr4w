module;

#include "pch.h"

export module vr4w:Awaiters;
import std;

namespace vr4w::details {

export auto ResumeOnLoop(HWND hwnd, UINT msg, bool async = true) {
  struct Awaiter {
    HWND LoopWnd;
    UINT LoopMsg;
    bool Async;

    constexpr auto await_ready() const noexcept { return false; }
    constexpr auto await_suspend(std::coroutine_handle<> handle) const noexcept {
      if (Async) {
        PostMessage(LoopWnd, LoopMsg, 0, reinterpret_cast<LPARAM>(handle.address()));
      } else {
        SendMessage(LoopWnd, LoopMsg, 0, reinterpret_cast<LPARAM>(handle.address()));
      }
    }
    constexpr void await_resume() const noexcept {};
  };
  return Awaiter{.LoopWnd = hwnd, .LoopMsg = msg, .Async = async};
}

export auto InvokeOnLoop(LPARAM lparam) {
  auto handle = std::coroutine_handle<>::from_address(reinterpret_cast<void*>(lparam));
  handle.resume();
}

}  // namespace vr4w::details
