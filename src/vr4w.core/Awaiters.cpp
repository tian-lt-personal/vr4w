// pch
#include "pch.h"

// decl
#include "Awaiters.hpp"

namespace vr4w::impl {

namespace details {

void SchedueLoopTask(HWND hwnd, UINT msg, void* handle) {
  PostMessage(hwnd, msg, 0, reinterpret_cast<LPARAM>(handle));
}

}  // namespace details

void InvokeOnLoop(LPARAM lparam) {
  auto handle = std::coroutine_handle<>::from_address(reinterpret_cast<void*>(lparam));
  handle.resume();
}

}  // namespace vr4w::impl
