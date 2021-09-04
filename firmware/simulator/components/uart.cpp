#include "uart.h"

#include <iostream>

namespace threeboard {
namespace simulator {

using namespace std::placeholders;

Uart::Uart(Simavr *simavr, UIDelegate *ui_delegate)
    : ui_delegate_(ui_delegate) {
  // Disable the default UART stdio dump in simavr.
  uint32_t flags = 0;
  simavr->InvokeIoctl(UART_GET_FLAGS, &flags);
  flags &= ~UART_FLAG_STDIO;
  simavr->InvokeIoctl(UART_SET_FLAGS, &flags);

  input_callback_ = std::make_unique<UartOutputCallback>(
      std::bind(&Uart::LogCharacterInputCallback, this, _1));
  input_lifetime_ = simavr->RegisterUartOutputCallback(input_callback_.get());
}

void Uart::LogCharacterInputCallback(uint8_t value) {
  if (value == '\n') {
    ui_delegate_->HandleLogLine(log_buffer_);
    log_buffer_ = "";
  } else {
    log_buffer_ += value;
  }
}
}  // namespace simulator
}  // namespace threeboard