#include "simulator/simulator.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

// Used to test a single pin in a register.
inline bool IsEnabled(uint8_t reg, uint8_t pin) { return reg & (1 << pin); }

inline uint8_t SetBit(bool status, uint8_t idx) {
  return status ? (1 << idx) : 0;
}

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

inline void SetLedState(uint8_t &led, bool enabled) {
  if (enabled) {
    led = 5;
  } else if (led > 0) {
    led -= 1;
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
  sim_thread_.join();
}

void Simulator::RunAsync() {
  // Start a new thread to run the simulator and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Simulator::InternalRunAsync, this);
}

void Simulator::RunFullSpeedAsync() {
  simavr_->DisableSleep();
  RunAsync();
}

void Simulator::Reset() { should_reset_ = true; }

DeviceState Simulator::GetDeviceState() {
  auto state = device_state_;
  device_state_.usb_buffer.clear();
  return state;
}

SimulatorState Simulator::GetSimulatorState() {
  SimulatorState state;
  state.cpu_state = simavr_->GetState();
  state.gdb_enabled = (simavr_->GetGdbPort() > 0);
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

uint64_t Simulator::GetCurrentCpuCycle() { return simavr_->GetCycle(); }

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
  // If the Uart class is initialized and in scope, it will handle logging
  // output from the simulator.
  uart_ = std::make_unique<Uart>(simavr_, ui_delegate);
}

void Simulator::HandleUsbOutput(uint8_t mod_code, uint8_t key_code) {
  bool capitalise = false;
  // Check for L_SHIFT and R_SHIFT.
  if ((mod_code & 0x22) > 0 && (mod_code & ~0x22) == 0) {
    capitalise = true;
  } else if (mod_code != 0) {
    // Ignore and reject any non-shift modcodes.
    return;
  }
  char c;
  if (key_code >= 0x04 && key_code <= 0x1d) {
    c = key_code + 0x5d;
    if (capitalise) {
      c -= 0x20;
    }
  } else if (key_code == 0x2a) {
    c = ' ';
  } else if (key_code == 0x2d) {
    c = '-';
  } else if (key_code == 0x36) {
    c = ',';
  } else if (key_code == 0x37) {
    c = '.';
  } else {
    // Ignore unsupported characters.
    // TODO: support some special characters!
    return;
  }
  device_state_.usb_buffer += c;
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
    device_state_.led_err = IsEnabled(simavr_->GetData(PORTB), 6) &&
                            !IsEnabled(simavr_->GetData(PORTC), 6);
  } else if (row == 1) {
    device_state_.bank_0 &= 0xF0;
    device_state_.bank_0 |= cols;
    device_state_.led_status = IsEnabled(simavr_->GetData(PORTC), 6) &&
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
    // frequency. It's a difficult problem so it's not perfect (and simavr
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