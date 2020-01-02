#include "native/native_impl.h"
#include "threeboard.h"

using namespace threeboard;

int main() {
  // The Native interface is used to abstract away all "native" code (e.g.
  // interrupt setup code, setting various pin port values). This is the only
  // place that NativeImpl is injected. To keep all other components testable,
  // they all use the Native interface. The only two untestable components are
  // therefore this main file, and the NativeImpl itself, since they can only
  // compile for AVR.
  auto native_impl = native::NativeImpl::Get();

  // Set up the remaining objects to inject into the Threeboard instance. These
  // could be constructed within the instance, but injecting them makes testing
  // easier since we can inject mocks for tets.
  EventHandler event_handler(native_impl);
  LedController led_controller(native_impl);
  KeyController key_controller(native_impl, &event_handler);

  Threeboard threeboard(native_impl, &event_handler, &led_controller,
                        &key_controller);

  // Run the firmware event loop. This will run forever.
  threeboard.Run();
}
