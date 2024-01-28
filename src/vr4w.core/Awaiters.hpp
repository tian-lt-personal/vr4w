#pragma once

#include <windef.h>

#include <coroutine>

namespace vr4w::impl {

namespace details {

void SchedueLoopTask(HWND hwnd, UINT msg, bool async, void* handle);

}  // namespace details

inline auto ResumeOnLoop(HWND hwnd, UINT msg, bool async = true) {
  struct Awaiter {
    HWND LoopWnd;
    UINT LoopMsg;
    bool Async;

    constexpr auto await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) const noexcept {
      details::SchedueLoopTask(LoopWnd, LoopMsg, Async, handle.address());
    }
    constexpr void await_resume() const noexcept {};
  };
  return Awaiter{.LoopWnd = hwnd, .LoopMsg = msg, .Async = async};
}

void InvokeOnLoop(LPARAM lparam);

}  // namespace vr4w::impl
