#include "simulator.h"

#include "simavr/avr_ioport.h"
#include "simavr/sim_avr.h"
#include "simavr/sim_elf.h"
#include <iostream>

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

// Relative firmware path. Bazel will guarantee this is built since it's listed
// as a dependency.
const std::string firmware_file = "simulator/threeboard_sim_binary.elf";

// A C-compatible "trampoline"-style callback which bounces the callback to the
// C++ member function (of type MemberCallback) passed in param. It also commits
// the write to the correct data register so the downstream callback doesn't
// need to care about that.
void WriteCallbackWrapper(struct avr_t *avr, avr_io_addr_t addr, uint8_t v,
                          void *param) {
  avr->data[addr] = v;
  Simulator::PortCallback *callback = (Simulator::PortCallback *)param;
  (*callback)(addr, v);
}
} // namespace

void Simulator::RunAsync() {
  // Initialise firmware from ELF file.
  elf_firmware_t f;
  if (elf_read_firmware(firmware_file.c_str(), &f)) {
    std::cout << "Failed to read ELF firmware '" << firmware_file << "'"
              << std::endl;
    exit(1);
  }
  std::cout << "Loaded firmware '" << firmware_file << "'" << std::endl;
  avr_t *avr = avr_make_mcu_by_name(f.mmcu);
  if (!avr) {
    std::cout << "Unknown MMCU: '" << f.mmcu << "'" << std::endl;
    exit(1);
  }
  avr_init(avr);
  avr_load_firmware(avr, &f);
  avr_ = std::unique_ptr<avr_t>(avr);
  mcu_ = std::unique_ptr<mcu_t>((mcu_t *)avr);

  // Wire up port IO write notifications as IRQ events with callbacks through
  // the WriteCallbackWrapper trampoline.
  port_callback_ = std::make_unique<PortCallback>(
      std::bind(&Simulator::InternalPortCallback, this, _1, _2));
  void *cb_ptr = (void *)port_callback_.get();
  avr_register_io_write(avr_.get(), mcu_->portb.r_port, &WriteCallbackWrapper,
                        cb_ptr);
  avr_register_io_write(avr_.get(), mcu_->portc.r_port, &WriteCallbackWrapper,
                        cb_ptr);
  avr_register_io_write(avr_.get(), mcu_->portd.r_port, &WriteCallbackWrapper,
                        cb_ptr);

  // Start a new thread to run the AVR firmware and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Simulator::RunDetached, this);
}

void Simulator::RunDetached() {
  std::cout << "Running detatched" << std::endl;
  while (is_running_) {
    avr_run(avr_.get());
  }
}

void Simulator::InternalPortCallback(avr_io_addr_t addr, uint8_t val) {
  if (addr == mcu_->portb.r_port) {
    std::cout << "PORTB: ";
    portb_ = val;
  } else if (addr == mcu_->portc.r_port) {
    std::cout << "PORTC: ";
    portc_ = val;
  } else if (addr == mcu_->portd.r_port) {
    std::cout << "PORTD: ";
    portd_ = val;
  }
  std::cout << "addr = " << AVR_DATA_TO_IO(addr) << ", val = " << (int)val
            << std::endl;
}
} // namespace simulator
} // namespace threeboard
