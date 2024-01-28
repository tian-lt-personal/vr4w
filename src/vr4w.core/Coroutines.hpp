#pragma once

#include <cassert>
#include <coroutine>
#include <exception>
#include <variant>

namespace vr4w {

namespace details {

struct TaskPromiseBase {
  struct FinalAwaiter {
    constexpr bool await_ready() const noexcept { return false; }
    template <class P>
    constexpr auto await_suspend(std::coroutine_handle<P> handle) noexcept {
      static_assert(std::is_base_of_v<details::TaskPromiseBase, P>);
      return handle.promise().cont_;
    }
    constexpr void await_resume() const noexcept {}
  };
};

}  // namespace details

struct FireAndForget {
  struct promise_type {
    auto get_return_object() const noexcept { return FireAndForget{}; }
    void return_void() const noexcept {};
    auto initial_suspend() const noexcept { return std::suspend_never{}; }
    auto final_suspend() const noexcept { return std::suspend_never{}; }
    void unhandled_exception() const noexcept { std::terminate(); };
  };
};

template <class T = void>
class Task {
 public:
  Task(Task&& rhs) noexcept : coro_(std::exchange(rhs.coro_, nullptr)) {}
  Task& operator=(Task&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }
    coro_ = std::exchange(rhs.coro_, nullptr);
    return *this;
  }
  ~Task() {
    if (coro_ != nullptr) {
      coro_.destroy();
    }
  }

 public:
  struct promise_type : details::TaskPromiseBase {
    constexpr auto get_return_object() noexcept {
      return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    constexpr auto initial_suspend() const noexcept { return std::suspend_always{}; }
    constexpr auto final_suspend() const noexcept { return FinalAwaiter{}; }

    template <class V>
    void return_value(V&& value) noexcept(std::is_nothrow_constructible_v<T, V&&>)
      requires(std::is_convertible_v<V &&, T>)
    {
      value_.template emplace<T>(std::forward<V>(value));
    }
    constexpr void unhandled_exception() const noexcept { std::terminate(); }

    std::coroutine_handle<> cont_;
    std::variant<std::monostate, T> value_ = std::monostate{};
  };  // class promise_type

  auto operator co_await() && noexcept {
    struct Awaiter {
      constexpr bool await_ready() const noexcept { return false; }
      constexpr auto await_suspend(std::coroutine_handle<> cont) noexcept {
        coro_.promise().cont_ = cont;
        return coro_;
      }
      constexpr T await_resume() const noexcept {
        assert(!std::holds_alternative<std::monostate>(coro_.promise().value_));
        return std::move(std::get<T>(coro_.promise().value_));
      }
      constexpr explicit Awaiter(std::coroutine_handle<promise_type> handle) noexcept
          : coro_(handle) {}
      std::coroutine_handle<promise_type> coro_;
    };
    return Awaiter{coro_};
  }

 private:
  constexpr explicit Task(std::coroutine_handle<promise_type> handle) noexcept : coro_(handle) {}
  std::coroutine_handle<promise_type> coro_ = nullptr;
};  // class Task<T>

template <>
struct Task<void> {
  struct promise_type : details::TaskPromiseBase {
    constexpr auto get_return_object() noexcept {
      return Task<void>{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    constexpr auto initial_suspend() const noexcept { return std::suspend_always{}; }
    constexpr auto final_suspend() const noexcept { return FinalAwaiter{}; }
    constexpr void return_void() const noexcept {}
    void unhandled_exception() const noexcept { std::terminate(); }

    std::coroutine_handle<> cont_;
  };

  auto operator co_await() && noexcept {
    struct Awaiter {
      constexpr bool await_ready() const noexcept { return false; }
      auto await_suspend(std::coroutine_handle<> cont) noexcept {
        coro_.promise().cont_ = cont;
        return coro_;
      }
      constexpr void await_resume() const noexcept {}
      constexpr explicit Awaiter(std::coroutine_handle<promise_type> handle) noexcept
          : coro_(handle) {}
      std::coroutine_handle<promise_type> coro_;
    };
    return Awaiter{coro_};
  }

 private:
  constexpr explicit Task(std::coroutine_handle<promise_type> handle) noexcept : coro_(handle) {}
  std::coroutine_handle<promise_type> coro_ = nullptr;
};

}  // namespace vr4w
