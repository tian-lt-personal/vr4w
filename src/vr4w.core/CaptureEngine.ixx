module;

// pch
#include "pch.h"

export module vr4w;

import std;

export namespace vr4w {

export class CaptureEngine {
 public:
  CaptureEngine() : engineThrd_([this] { EngineThread(); }) {}
  void Stop() {}

 private:
  void EngineThread() {}

 private:
  std::jthread engineThrd_;
};

}  // namespace vr4w
