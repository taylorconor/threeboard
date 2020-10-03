#include "uart.h"

#include <iostream>

namespace threeboard {
namespace simulator {

using namespace std::placeholders;

Uart::Uart(Simavr *simavr, SimulatorDelegate *simulator_delegate)
    : simulator_delegate_(simulator_delegate) {
  // Disable the default UART stdio dump in simavr.
  uint32_t flags = 0;
  simavr->InvokeIoctl(UART_GET_FLAGS, &flags);
  flags &= ~UART_FLAG_STDIO;
  simavr->InvokeIoctl(UART_SET_FLAGS, &flags);

  input_callback_ = std::make_unique<UartOutputCallback>(
      std::bind(&Uart::LogCharacterInputCallback, this, _1));
  simavr->RegisterUartOutputCallback(input_callback_.get());
}

void Uart::LogCharacterInputCallback(uint8_t value) {
  if (value == '\n') {
    simulator_delegate_->HandleUartLogLine(log_buffer_);
    log_buffer_ = "";
  } else {
    log_buffer_ += value;
  }
}
} // namespace simulator
} // namespace threeboard