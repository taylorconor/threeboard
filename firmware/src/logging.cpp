#include "src/logging.h"

#include <stdarg.h>
#include <stdio.h>

#include "src/util/util.h"

namespace threeboard {
namespace {

// Transmit a single byte over USART1.
__force_inline void Transmit(native::Native *native, char c) {
  // Send a single byte of log data to the USART1 I/O data register.
  native->SetUDR1(c);
}
}  // namespace

// static.
// This is defined as static so that all usages of LOG don't need to provide
// their own native instance to the logger. Instead, bootstrap.cpp intialises
// the logger with this statically after creating the native instance.
native::Native *Logging::native_;

// static.
void Logging::Init(native::Native *native) { native_ = native; }

void Logging::Log(const char *fmt, ...) {
  /*va_list va;
  va_start(va, fmt);
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), fmt, va);
  va_end(va);

  for (int i = 0; buffer[i] != 0; ++i) {
    Transmit(native_, buffer[i]);
  }
  Transmit(native_, '\n');*/
}

}  // namespace threeboard