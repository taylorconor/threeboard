#include "src/bootstrap.h"

#include "src/logging.h"
#include "src/native/native_impl.h"
#include "src/threeboard.h"
#include "src/usb/usb_controller_impl.h"
#include "src/util/error_handler_proxy.h"

using namespace threeboard;

void RunThreeboardEventLoop() {
  // The native interface is used to abstract away all "native" code (e.g.
  // interrupt setup code, setting various pin port values). This is the only
  // place that NativeImpl is injected. To keep all other components testable,
  // they all use the Native interface.
  native::NativeImpl native_impl;
  Logging::Init(&native_impl);
  LOG("Native layer initialised");
  LOG("NativeImpl: %p", &native_impl);

  util::ErrorHandlerProxy error_handler_proxy;

  // Similar to how we construct native_impl above, this is the only place where
  // UsbImpl is injected, so as to enable other components to be testable with a
  // mocked USB implementation.
  auto usb_controller_impl =
      usb::UsbControllerImpl(&native_impl, &error_handler_proxy);

  // Set up the remaining objects to inject into the Threeboard instance. These
  // could be constructed within the instance, but injecting them makes testing
  // easier since we can inject mocks into the Threeboard class for tests.
  storage::StorageController storage_controller(&native_impl);
  EventBuffer event_buffer;
  LedController led_controller(&native_impl);
  KeyController key_controller(&native_impl, &event_buffer);

  // The `threeboard` object is a high-level class responsible for coordinating
  // all threeboard components composed into it.
  Threeboard threeboard(&native_impl, &event_buffer, &usb_controller_impl,
                        &storage_controller, &led_controller, &key_controller);

  error_handler_proxy.SetImpl(&threeboard);

  // Run the firmware event loop. This will run forever.
  threeboard.RunEventLoop();
}
