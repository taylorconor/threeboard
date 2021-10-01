#include "uart.h"

namespace threeboard {
namespace simulator {

using namespace std::placeholders;

Uart::Uart(Simavr *simavr, UIDelegate *ui_delegate, std::ofstream *log_stream)
    : ui_delegate_(ui_delegate), log_stream_(log_stream) {
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
    if (ui_delegate_) {
      ui_delegate_->HandleLogLine(log_buffer_);
    }
    if (log_stream_) {
      *log_stream_ << log_buffer_ << std::endl;
    }
    log_buffer_ = "";
  } else {
    log_buffer_ += value;
  }
}
}  // namespace simulator
}  // namespace threeboard