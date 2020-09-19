#include "firmware.h"

#include <iostream>

#include "simavr/avr_ioport.h"
#include "simavr/sim_elf.h"
#include "simavr/sim_gdb.h"

namespace threeboard {
namespace simulator {
namespace {

// Relative firmware path. Bazel will guarantee this is built since it's listed
// as a dependency.
const std::string kFirmwareFile = "simulator/threeboard_sim_binary.elf";

} // namespace

Firmware::Firmware()
    : is_running_(false), mcu_(nullptr), should_reset_(false) {}

Firmware::~Firmware() {
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

void Firmware::RunAsync() {
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

  // Start a new thread to run the AVR firmware and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Firmware::RunDetached, this);
}

void Firmware::Reset() { should_reset_ = true; }

avr_t *Firmware::GetAvr() const { return avr_.get(); }

// TODO: assert here if is_running_ is false?
uint8_t Firmware::GetPortB() const { return avr_->data[PORTB]; }
uint8_t Firmware::GetPortC() const { return avr_->data[PORTC]; }
uint8_t Firmware::GetPortD() const { return avr_->data[PORTD]; }
uint8_t Firmware::GetPortF() const { return avr_->data[PORTF]; }

void Firmware::SetPinB(uint8_t pin, bool enabled) {
  avr_->data[PINB] &= ~(1 << pin);
  if (enabled) {
    avr_->data[PINB] |= 1 << pin;
  }
}

int Firmware::GetCpuState() const { return avr_->state; }

uint64_t Firmware::GetCpuCycleCount() const { return avr_->cycle; }

bool Firmware::IsGdbEnabled() const { return avr_->gdb_port != 0; }

void Firmware::EnableGdb(uint16_t port) const {
  avr_->gdb_port = port;
  avr_->state = cpu_Stopped;
  avr_gdb_init(avr_.get());
}

void Firmware::DisableGdb() const {
  avr_deinit_gdb(avr_.get());
  avr_->state = cpu_Running;
  avr_->gdb_port = 0;
}

void Firmware::RunDetached() {
  while (is_running_ && avr_->state != cpu_Done && avr_->state != cpu_Crashed) {
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
  if (avr_->state == cpu_Done || avr_->state == cpu_Crashed) {
    std::cout << "Simulator finished with state "
              << (avr_->state == cpu_Done ? "DONE" : "CRASHED") << std::endl;
  }
}
} // namespace simulator
} // namespace threeboard
