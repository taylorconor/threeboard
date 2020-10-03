#pragma once

#include "src/native/native.h"

// TODO: disable this when not running within the simulator.
#define LOG Logging()

namespace threeboard {

// A logging class designed to send logs from the firmware to the simavr
// simulator. This should never be included in non-simulator firmware files,
// since it will introduce more clock cycles, and the UART register setup has
// not been tested on physical hardware.
class Logging {
public:
  // Must be called before any logging is performed to set the native instance.
  static void Init(native::Native *native);

  ~Logging();
  Logging &operator<<(const char *msg);

private:
  static native::Native *native_;
};
} // namespace threeboard