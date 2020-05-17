#pragma once

#include "simavr/sim_avr.h"
#include <thread>

namespace threeboard {
namespace simulator {

class Simulator {
public:
  using WriteCallback = std::function<void(avr_io_addr_t, uint8_t)>;
  using ReadCallback = std::function<uint8_t(avr_io_addr_t)>;

  ~Simulator();

  void RunAsync();

  void Reset();

  // Retrieve ports containing output pins.
  uint8_t GetPortB() const;
  uint8_t GetPortC() const;
  uint8_t GetPortD() const;
  uint8_t GetPortF() const;

  // Set ports containing input pins.
  void SetPinB(uint8_t, bool);

  // Returns a reference to the simulator state.
  const int &GetState() const;

  // Returns the simulator cycle count.
  const uint64_t &GetCycleCount() const;

private:
  void RunDetached();
  void InternalWriteCallback(avr_io_addr_t, uint8_t);
  uint8_t InternalReadCallback(avr_io_addr_t);

  // Ports containing output pins.
  uint8_t portb_;
  uint8_t portc_;
  uint8_t portd_;
  uint8_t portf_;

  // Ports containing input pins.
  uint8_t pinb_ = 0xFF;

  std::atomic<bool> is_running_;
  std::atomic<bool> should_reset_;

  std::unique_ptr<WriteCallback> write_callback_;
  std::unique_ptr<ReadCallback> read_callback_;
  std::unique_ptr<avr_t> avr_;
  std::thread sim_thread_;
};
} // namespace simulator
} // namespace threeboard
