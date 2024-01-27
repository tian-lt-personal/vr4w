import std;
import vr4w;

int main() {
  auto devices = vr4w::GetAllDevices();
  vr4w::CaptureEngine engine;
  engine.Connect(devices[0].Index);
}
