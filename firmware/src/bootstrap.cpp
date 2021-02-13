#include "src/bootstrap.h"

#include "src/logging.h"
#include "src/native/i2c.h"
#include "src/native/native_impl.h"
#include "src/threeboard.h"
#include "src/usb/usb_impl.h"

using namespace threeboard;

void RunThreeboard() {
  // The native interface is used to abstract away all "native" code (e.g.
  // interrupt setup code, setting various pin port values). This is the only
  // place that NativeImpl is injected. To keep all other components testable,
  // they all use the Native interface.
  auto *native_impl = native::NativeImpl::Get();
  Logging::Init(native_impl);
  LOG("Native layer initialised");

  // Similar to how we construct native_impl above, this is the only place where
  // UsbImpl is injected, so as to enable other components to be testable with a
  // mocked USB implementation.
  auto usb_impl = usb::UsbImpl(native_impl);

  // Set up the remaining objects to inject into the Threeboard instance. These
  // could be constructed within the instance, but injecting them makes testing
  // easier since we can inject mocks for tests.
  EventBuffer event_buffer;
  LedController led_controller(native_impl);
  KeyController key_controller(native_impl, &event_buffer);

  // TODO: pass the i2c instance into the threeboard. All this code does is
  // prove that read after write works.
  native::I2C i2c(native_impl);
  uint8_t data[] = {1, 2, 3, 4, 5};
  i2c.Write(0, 0, data, 5);
  LOG("Finished i2c write");
  uint8_t read_data[] = {0, 0, 0, 0, 0};
  i2c.Read(0, 0, read_data, 5);
  LOG("Finished i2c read: {%d,%d,%d,%d,%d}", read_data[0], read_data[1],
      read_data[2], read_data[3], read_data[4]);

  // The `threeboard` object is a high-level class responsible for coordinating
  // all threeboard components composed into it.
  Threeboard threeboard(native_impl, &usb_impl, &event_buffer, &led_controller,
                        &key_controller);

  // Run the firmware event loop. This will run forever.
  threeboard.Run();
}
