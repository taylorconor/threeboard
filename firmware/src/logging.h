#pragma once

#include "src/native/native.h"

// TODO: disable these when not running within the simulator.
#define LOG(fmt, args...) Logging::Log(fmt, ##args);
#define LOG_ONCE(fmt, args...)                                                 \
  static __attribute__((unused)) bool ___tb_logging_lock = []() {              \
    Logging::Log(fmt, ##args);                                                 \
    return true;                                                               \
  }();

namespace threeboard {

// A logging class designed to send logs from the firmware to the simavr
// simulator. This WILL NOT WORK on actual hardware, it takes advantage of the
// fact that simavr doesn't care about USART register setup, baud rate
// specification etc. All simavr needs to receive USART bytes is for the bytes
// to be written to UDR1 as fast as possible.
class Logging {
public:
  // Must be called before any logging is performed to set the native instance.
  static void Init(native::Native *native);

  static void Log(const char *fmt, ...);

private:
  static native::Native *native_;
};
} // namespace threeboard