#pragma once

#include "src/native/native.h"

// TODO: disable these when not running within the simulator.
#define LOG(fmt, ...)                                                          \
  do {                                                                         \
    ::threeboard::Logging::Log(PSTR(fmt), ##__VA_ARGS__);                      \
  } while (0)

#define LOG_ONCE(fmt, ...)                                                     \
  do {                                                                         \
    static bool ___tb_has_logged = false;                                      \
    if (!___tb_has_logged) {                                                   \
      ___tb_has_logged = true;                                                 \
      LOG(fmt, ##__VA_ARGS__);                                                 \
    }                                                                          \
  } while (0)

namespace threeboard {

// A logging class designed to send logs from the firmware to the simavr
// simulator.
//
// This WILL NOT WORK on actual hardware, since it takes advantage of the fact
// that simavr doesn't care about USART register setup, baud rate specification
// etc. All simavr needs to receive USART bytes is for the bytes to be written
// to UDR1 as fast as possible.
class Logging {
public:
  // Must be called before any logging is performed to set the native
  // instance.
  static void Init(native::Native *native);

  static void Log(const char *fmt, ...);

private:
  static native::Native *native_;
};
} // namespace threeboard