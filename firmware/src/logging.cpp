#include "logging.h"

namespace threeboard {
namespace {

// Transmit a string over USART1. The string is assumed to be null-terminated.
void Transmit(native::Native *native, const char *log) {
  for (int i = 0; log[i] != 0; ++i) {
    // Wait for empty transmit buffer
    while (!(native->GetUCSR1A() & (1 << native::UDRE1)))
      ;
    // Clear transmission flag.
    native->SetUCSR1A(native->GetUCSR1A() | (1 << native::TXC1));
    // Send a single byte of log data to the USART1 I/O data register.
    native->SetUDR1(log[i]);
  }
}
} // namespace

// static.
// This is defined as static so that all usages of LOG don't need to provide
// their own native instance to the logger. Instead, bootstrap.cc intialises the
// logger with this statically after creating the native instance.
native::Native *Logging::native_;

// static.
void Logging::Init(native::Native *native) {
  Logging::native_ = native;

  // Enable UART transmitter only. No need for a receiver.
  native->SetUCSR1B(native->GetUCSR1B() | (1 << native::TXEN1));

  // Set frame format to just 8 data bits. We don't need any parity or stop bits
  // because the UART receiver in the simulator doesn't care.
  native->SetUCSR1C(native->GetUCSR1C() | (1 << native::UCSZ10));

  // Clear transmission flag.
  native->SetUCSR1A(native->GetUCSR1A() | (1 << native::TXC1));
}

Logging::~Logging() {
  // Assuming we're logging with the LOG macro, nce the log
  // line is finished its temporary Logging instance will be
  // destructed. We need to transmit a newline character to
  // tell simavr that this line is finished.
  Transmit(native_, "\n");
}

Logging &Logging::operator<<(const char *msg) {
  Transmit(native_, msg);
  return *this;
}

} // namespace threeboard