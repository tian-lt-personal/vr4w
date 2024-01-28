#include <Windows.h>

#include "CaptureEngine.hpp"

int main() {
  auto devices = vr4w::GetAllDevices();
  vr4w::CaptureEngine engine;
}
