#include "native/native_impl.h"
#include "threeboard.h"

using namespace threeboard;

int main() {
  // This is the only place that NativeImpl is injected. To keep all other
  // components testable, they all use the Native interface. The only two
  // untestable components are therefore this main file, and the NativeImpl
  // itself, since they can only compile for AVR.
  Threeboard threeboard(native::NativeImpl::Get());
  threeboard.Run();
}
