#pragma once

// std headers
#include <exception>

// com headers
#include <combaseapi.h>

inline void check_hresult(HRESULT hr) {
  if (FAILED(hr)) {
    std::terminate();
  }
}
