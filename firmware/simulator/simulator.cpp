#include "simulator.h"

#include "simavr/avr_ioport.h"
#include "simavr/sim_avr.h"
#include "simavr/sim_core_decl.h"
#include "simavr/sim_elf.h"
#include "simavr/sim_gdb.h"
#include <immintrin.h>
#include <iostream>
#include <unistd.h>

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

// Relative firmware path. Bazel will guarantee this is built since it's listed
// as a dependency.
const std::string kFirmwareFile = "simulator/threeboard_sim_binary.elf";

// C-compatible "trampoline"-style callback which bounces the callback to the
// C++ WriteCallback passed in param. It also commits the write to the correct
// data register so the downstream callback doesn't need to care about that.
void WriteCallbackWrapper(struct avr_t *avr, avr_io_addr_t addr, uint8_t v,
                          void *param) {
  avr->data[addr] = v;
  Simulator::WriteCallback *callback = (Simulator::WriteCallback *)param;
  (*callback)(addr, v);
}

// C-compatible "trampoline"-style callback which bounces the callback to the
// C++ ReadCallback passed in param.
uint8_t ReadCallbackWrapper(struct avr_t *avr, avr_io_addr_t addr,
                            void *param) {
  Simulator::ReadCallback *callback = (Simulator::ReadCallback *)param;
  return (*callback)(addr);
}
} // namespace

Simulator::~Simulator() {
  if (is_running_) {
    is_running_ = false;
    if (avr_) {
      avr_terminate(avr_.get());
    }
    if (sim_thread_.joinable()) {
      sim_thread_.join();
    }
    std::cout << "Simulator shut down successfully." << std::endl;
  }
}

void Simulator::RunAsync() {
  // Initialise firmware from ELF file.
  elf_firmware_t f;
  if (elf_read_firmware(kFirmwareFile.c_str(), &f)) {
    std::cout << "Failed to read ELF firmware '" << kFirmwareFile << "'"
              << std::endl;
    exit(1);
  }
  std::cout << "Loaded firmware '" << kFirmwareFile << "'" << std::endl;
  avr_t *avr = avr_make_mcu_by_name(f.mmcu);
  if (!avr) {
    std::cout << "Unknown MMCU: '" << f.mmcu << "'" << std::endl;
    exit(1);
  }
  avr_init(avr);
  avr_load_firmware(avr, &f);
  avr_ = std::unique_ptr<avr_t>(avr);
  mcu_ = (mcu_t *)avr_.get();

  write_callback_ = std::make_unique<WriteCallback>(
      std::bind(&Simulator::InternalWriteCallback, this, _1, _2));
  void *write_cb_ptr = (void *)write_callback_.get();
  avr_register_io_write(avr_.get(), PORTB, &WriteCallbackWrapper, write_cb_ptr);
  avr_register_io_write(avr_.get(), PORTC, &WriteCallbackWrapper, write_cb_ptr);
  avr_register_io_write(avr_.get(), PORTD, &WriteCallbackWrapper, write_cb_ptr);
  avr_register_io_write(avr_.get(), PORTF, &WriteCallbackWrapper, write_cb_ptr);

  read_callback_ = std::make_unique<ReadCallback>(
      std::bind(&Simulator::InternalReadCallback, this, _1));
  void *read_cb_ptr = (void *)read_callback_.get();

  // Set the read callback to be called when the firmware reads from PINB.
  // TODO: i want to use the built-in avr_register_io_read but it crashes here.
  // why?
  avr_io_addr_t a = AVR_DATA_TO_IO(PINB);
  avr_.get()->io[a].r.param = read_cb_ptr;
  avr_.get()->io[a].r.c = &ReadCallbackWrapper;

  // Start a new thread to run the AVR firmware and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Simulator::RunDetached, this);
}

void Simulator::Reset() { should_reset_ = true; }

uint8_t Simulator::GetPortB() const { return portb_; }
uint8_t Simulator::GetPortC() const { return portc_; }
uint8_t Simulator::GetPortD() const { return portd_; }
uint8_t Simulator::GetPortF() const { return portf_; }

void Simulator::SetPinB(uint8_t pin, bool enabled) {
  pinb_ &= ~(1 << pin);
  if (enabled) {
    pinb_ |= 1 << pin;
  }
}

const int &Simulator::GetState() const { return avr_->state; }

const uint64_t &Simulator::GetCycleCount() const { return avr_->cycle; }

void Simulator::EnableGdb(uint16_t port) const {
  avr_->gdb_port = port;
  avr_->state = cpu_Stopped;
  avr_gdb_init(avr_.get());
}

void Simulator::DisableGdb() const {
  avr_deinit_gdb(avr_.get());
  avr_->state = cpu_Running;
  avr_->gdb_port = 0;
}

void Simulator::RunDetached() {
  int state = cpu_Running;
  while (is_running_ && state != cpu_Done && state != cpu_Crashed) {
    if (should_reset_) {
      avr_reset(avr_.get());
      should_reset_ = false;
    }

    // Since the threeboard is entirely interrupt driven (the main runloop does
    // poll the keyboard state, but sleeps the CPU between interrupts), simavr
    // will attempt to match the simulator frequency to the target 16 MHz
    // frequency. It's a difficult problem so it's not perfect (and simavr
    // doesn't attempt to make it perfect), but in my experience you can
    // expect 17.5±1.5MHz.
    avr_run(avr_.get());
  }
  is_running_ = false;
  if (state == cpu_Done || state == cpu_Crashed) {
    std::cout << "Simulator finished with state "
              << (state == cpu_Done ? "DONE" : "CRASHED") << std::endl;
  }
}

void Simulator::InternalWriteCallback(avr_io_addr_t addr, uint8_t val) {
  if (addr == PORTB) {
    portb_ = val;
  } else if (addr == PORTC) {
    portc_ = val;
  } else if (addr == PORTD) {
    portd_ = val;
  } else if (addr == PORTF) {
    portf_ = val;
  }
}

uint8_t Simulator::InternalReadCallback(avr_io_addr_t addr) {
  if (addr == PINB) {
    return pinb_;
  }
  return 0;
}
} // namespace simulator
} // namespace threeboard
