#pragma once

#include "core/sim_megax.h"
#include "simavr/sim_avr.h"
#include <thread>

namespace threeboard {
namespace simulator {

class Simulator {
public:
  using PortCallback = std::function<void(avr_io_addr_t, uint8_t)>;

  void RunAsync();

private:
  void RunDetached();
  void InternalPortCallback(avr_io_addr_t addr, uint8_t v);

  volatile bool is_running_;
  volatile uint8_t portb_;
  volatile uint8_t portc_;
  volatile uint8_t portd_;

  std::unique_ptr<PortCallback> port_callback_;
  std::unique_ptr<avr_t> avr_;
  std::unique_ptr<mcu_t> mcu_;
  std::thread sim_thread_;
};
} // namespace simulator
} // namespace threeboard
