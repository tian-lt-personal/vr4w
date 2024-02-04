#pragma once

#include <windef.h>

#include <coroutine>

namespace vr4w::impl {

namespace details {

void SchedueLoopTask(HWND hwnd, UINT msg, void* handle);

}  // namespace details

inline auto ResumeOnLoop(HWND hwnd, UINT msg) {
  struct Awaiter {
    HWND LoopWnd;
    UINT LoopMsg;

    constexpr auto await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) const noexcept {
      details::SchedueLoopTask(LoopWnd, LoopMsg, handle.address());
    }
    constexpr void await_resume() const noexcept {};
  };
  return Awaiter{.LoopWnd = hwnd, .LoopMsg = msg};
}

void InvokeOnLoop(LPARAM lparam);

}  // namespace vr4w::impl
