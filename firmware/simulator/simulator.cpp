#include "simulator/simulator.h"

#include <vector>

#include "simulator/components/usb_keycodes.h"
#include "simulator/util/logging.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

// Used to test a single pin in a register.
inline bool IsEnabled(uint8_t reg, uint8_t pin) { return reg & (1 << pin); }

inline uint16_t GetSramUsage(Simavr *simavr) {
  // Memory is laid out as follows in the atmega32u4:
  // |- registers -| |- ioports -| |- .data -|- .bss -|- << stack -----|
  // |--- 32 B ----| |-- 255 B --| |-------------- 2.5 K --------------|
  //               ^             ^                                     ^
  //             0x1F          0xFF                                  0xAFF
  // |----------------------- ramsize = 2815 B ------------------------|
  double usage = simavr->GetDataSectionSize() + simavr->GetBssSectionSize() +
                 simavr->GetRamSize() - simavr->GetStackPointer();
  return (usage / (simavr->GetRamSize() - 0xFF)) * 100;
}

inline void SetPinB(Simavr *simavr, uint8_t pin, bool enabled) {
  simavr->SetData(PINB, (simavr->GetData(PINB) & ~(1 << pin)));
  if (enabled) {
    simavr->SetData(PINB, (simavr->GetData(PINB) | (1 << pin)));
  }
}

std::vector<char> GetKeycodes(const Keypress &keypress) {
  switch (keypress) {
    case Keypress::X:
      return {'a'};
    case Keypress::Y:
      return {'s'};
    case Keypress::Z:
      return {'d'};
    case Keypress::XY:
      return {'a', 's'};
    case Keypress::XZ:
      return {'a', 'd'};
    case Keypress::YZ:
      return {'s', 'd'};
    case Keypress::XYZ:
      return {'a', 's', 'd'};
    default:
      return {};
  }
}
}  // namespace

Simulator::Simulator(Simavr *simavr, StateStorage *state_storage)
    : simavr_(simavr),
      is_running_(false),
      should_reset_(false),
      usb_host_(simavr_, this),
      eeprom0_(simavr_, state_storage, I2cEeprom::Instance::EEPROM_0) {
  portb_write_callback_ = std::make_unique<PortWriteCallback>(
      std::bind(&Simulator::HandlePortWrite, this, PORTB, _1));
  simavr_->RegisterPortBWriteCallback(portb_write_callback_.get());
  portd_write_callback_ = std::make_unique<PortWriteCallback>(
      std::bind(&Simulator::HandlePortWrite, this, PORTD, _1));
  simavr_->RegisterPortDWriteCallback(portd_write_callback_.get());
}

Simulator::~Simulator() {
  is_running_ = false;
  if (sim_thread_.joinable()) {
    sim_thread_.join();
  }
  if (log_stream_.is_open()) {
    log_stream_.close();
  }
}

void Simulator::RunAsync() {
  // Start a new thread to run the simulator and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Simulator::InternalRunAsync, this);
}

void Simulator::Reset() { should_reset_ = true; }

DeviceState Simulator::GetDeviceState() {
  auto state = device_state_;
  device_state_.usb_buffer.clear();
  return state;
}

SimulatorState Simulator::GetSimulatorState() const {
  SimulatorState state;
  state.cpu_state = simavr_->GetState();
  state.gdb_enabled = (simavr_->GetGdbPort() > 0);
  state.usb_attached = usb_host_.IsAttached();
  state.sram_usage = GetSramUsage(simavr_);
  state.data_section_size = simavr_->GetDataSectionSize();
  state.bss_section_size = simavr_->GetBssSectionSize();
  state.stack_size = simavr_->GetRamSize() - simavr_->GetStackPointer();
  return state;
}

void Simulator::HandleKeypress(char key, bool state) {
  if (key == 'a') {
    // Switch 1 - maps to PB2.
    SetPinB(simavr_, 2, !state);
  } else if (key == 's') {
    // Switch 2 - maps to PB3.
    SetPinB(simavr_, 3, !state);
  } else if (key == 'd') {
    // Switch 3 - maps to PB1.
    SetPinB(simavr_, 1, !state);
  }
}

void Simulator::HandleKeypress(const Keypress &keypress, bool state) {
  for (char key : GetKeycodes(keypress)) {
    HandleKeypress(key, state);
  }
}

void Simulator::WaitForUsbOutput(const std::chrono::milliseconds &timeout) {
  auto start = std::chrono::system_clock::now();
  while (timeout > std::chrono::system_clock::now() - start) {
    auto previous_output_time = last_usb_output_;
    // Sleep for double the USB endpoint polling time.
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    if (previous_output_time == last_usb_output_) {
      return;
    }
  }
}

uint64_t Simulator::GetCurrentCpuCycle() const { return simavr_->GetCycle(); }

void Simulator::ToggleGdb(uint16_t port) const {
  if (simavr_->GetGdbPort() == 0) {
    simavr_->SetGdbPort(port);
    simavr_->SetState(STOPPED);
    simavr_->InitGdb();
  } else {
    simavr_->DeinitGdb();
    simavr_->SetState(RUNNING);
    simavr_->SetGdbPort(0);
  }
}

void Simulator::EnableLogging(UIDelegate *ui_delegate) {
  char log_file[L_tmpnam];
  if (std::tmpnam(log_file)) {
    log_file_path_ = std::string(log_file);
    log_stream_.open(log_file_path_, std::ios_base::app);
  } else {
    std::cout << "Failed to create log file" << std::endl;
    exit(0);
  }
  std::cout << "Using log file '" << log_file_path_ << "'." << std::endl;

  // If the Uart class is initialized and in scope, it will handle logging
  // output from the simulated firmware.
  uart_ = std::make_unique<Uart>(simavr_, ui_delegate, &log_stream_);
  // Other log output (from the simulator itself, or from simavr) are handled
  // using the Logging singleton, which must be initialised separately.
  Logging::Init(ui_delegate, &log_stream_);
}

std::string Simulator::GetLogFile() const { return log_file_path_; }

void Simulator::HandleUsbOutput(uint8_t mod_code, uint8_t key_code) {
  last_usb_output_ = std::chrono::system_clock::now();
  char c = FromUsbKeycodes(key_code, mod_code);
  if (std::isprint(c)) {
    device_state_.usb_buffer.push_back(c);
  }
}

void Simulator::HandlePortWrite(uint8_t port, uint8_t value) {
  if (port == PORTB) {
    if (value & 0b00100000) {
      UpdateLedState(4);
    } else if (value & 0b00010000) {
      UpdateLedState(1);
    }
  } else if (port == PORTD) {
    if (value & 0b10000000) {
      UpdateLedState(0);
    } else if (value & 0b01000000) {
      UpdateLedState(2);
    } else if (value & 0b00010000) {
      UpdateLedState(3);
    }
  }
}

void Simulator::UpdateLedState(uint8_t row) {
  bool pf0 = !(simavr_->GetData(PORTF) & (1 << 0));
  bool pf1 = !(simavr_->GetData(PORTF) & (1 << 1));
  bool pf4 = !(simavr_->GetData(PORTF) & (1 << 4));
  bool pf5 = !(simavr_->GetData(PORTF) & (1 << 5));
  uint8_t cols = (pf0 << 3) | (pf1 << 2) | (pf4 << 1) | pf5;

  if (row == 0) {
    device_state_.bank_0 &= 0x0F;
    device_state_.bank_0 |= cols << 4;
    device_state_.led_status = IsEnabled(simavr_->GetData(PORTB), 6) &&
                               !IsEnabled(simavr_->GetData(PORTC), 6);
  } else if (row == 1) {
    device_state_.bank_0 &= 0xF0;
    device_state_.bank_0 |= cols;
    device_state_.led_err = IsEnabled(simavr_->GetData(PORTC), 6) &&
                            !IsEnabled(simavr_->GetData(PORTB), 6);
  } else if (row == 2) {
    device_state_.bank_1 &= 0x0F;
    device_state_.bank_1 |= cols << 4;
  } else if (row == 3) {
    device_state_.bank_1 &= 0xF0;
    device_state_.bank_1 |= cols;
  } else if (row == 4) {
    device_state_.led_r = pf0;
    device_state_.led_g = pf1;
    device_state_.led_b = pf4;
    device_state_.led_prog = pf5;
  }
}

void Simulator::InternalRunAsync() {
  while (is_running_ && simavr_->GetState() != DONE &&
         simavr_->GetState() != CRASHED) {
    if (should_reset_) {
      simavr_->Reset();
      should_reset_ = false;
    }

    // Since the threeboard is entirely interrupt driven (the main runloop does
    // poll the keyboard state, but sleeps the CPU between interrupts), simavr
    // will attempt to match the simulator frequency to the target 16 MHz
    // frequency. It's a difficult problem, so it's not perfect (and simavr
    // doesn't attempt to make it perfect), but in my experience you can
    // expect 17.5±1.5MHz.
    simavr_->Run();
  }
  is_running_ = false;
  if (simavr_->GetState() == DONE || simavr_->GetState() == CRASHED) {
    std::cout << "Simulator finished with state "
              << (simavr_->GetState() == DONE ? "DONE" : "CRASHED")
              << std::endl;
  }
}

}  // namespace simulator
}  // namespace threeboard