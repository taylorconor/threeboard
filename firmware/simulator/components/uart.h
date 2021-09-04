#pragma once

#include <memory>
#include <string>

#include "simulator/simavr/simavr.h"
#include "simulator/ui/ui_delegate.h"

namespace threeboard {
namespace simulator {

// A class to handle extraction of logs from the firmware while running within
// the simulator.
class Uart {
 public:
  explicit Uart(Simavr *simavr, UIDelegate *ui_delegate);

 private:
  void LogCharacterInputCallback(uint8_t value);

  UIDelegate *ui_delegate_;
  std::unique_ptr<UartOutputCallback> input_callback_;
  std::unique_ptr<Lifetime> input_lifetime_;
  std::string log_buffer_;
};
}  // namespace simulator
}  // namespace threeboard
