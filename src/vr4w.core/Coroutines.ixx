module;

#include "pch.h"

export module vr4w:Coroutines;
import std;

namespace vr4w::details {

export struct FireAndForget {
  struct promise_type {
    auto get_return_object() const noexcept { return FireAndForget{}; }
    void return_void() const noexcept {};
    auto initial_suspend() const noexcept { return std::suspend_never{}; }
    auto final_suspend() const noexcept { return std::suspend_never{}; }
    void unhandled_exception() const noexcept { std::terminate(); };
  };
};

}  // namespace vr4w::details
