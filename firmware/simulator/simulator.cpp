#include "simulator/simulator.h"

namespace threeboard {
namespace simulator {
namespace {

// Used to test a single pin in a register.
inline bool IsEnabled(uint8_t reg, uint8_t pin) { return reg & (1 << pin); }

inline uint8_t SetBit(bool status, uint8_t idx) {
  return status ? (1 << idx) : 0;
}

inline bool ShouldCapitalise(uint8_t modcode) { return modcode & 0b00100010; }

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
      eeprom0_(simavr_, state_storage, I2cEeprom::Instance::EEPROM_0) {}

Simulator::~Simulator() {
  is_running_ = false;
  sim_thread_.join();
  state_update_thread_.join();
}

void Simulator::RunAsync() {
  // Start a new thread to run the simulator and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Simulator::InternalRunAsync, this);
  state_update_thread_ = std::thread(&Simulator::UpdateLedState, this);
}

void Simulator::Reset() { should_reset_ = true; }

DeviceState Simulator::GetDeviceState() {
  DeviceState state;
  state.led_r = r_;
  state.led_g = g_;
  state.led_b = b_;
  state.led_prog = prog_;
  state.led_err = err_;
  state.led_status = status_;
  for (int i = 0; i < 8; ++i) {
    state.bank_0 |= !!bank0_[i] << i;
    state.bank_1 |= !!bank1_[i] << i;
  }
  state.usb_buffer = usb_buffer_;
  usb_buffer_.clear();
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
  char c;
  if (key_code >= 0x04 && key_code <= 0x1d) {
    c = key_code + 0x5d;
    if (ShouldCapitalise(mod_code)) {
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
  usb_buffer_ += c;
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

void Simulator::UpdateLedState() {
  while (is_running_) {
    const uint8_t portb = simavr_->GetData(PORTB);
    const uint8_t portc = simavr_->GetData(PORTC);
    const uint8_t portd = simavr_->GetData(PORTD);
    const uint8_t portf = simavr_->GetData(PORTF);

    // The ERR/STATUS LED pair for threeboard v1 is wired as follows:
    // status0: PB6, ERR
    // status1: PC6, STATUS
    if (IsEnabled(portb, 6) && !IsEnabled(portc, 6)) {
      SetLedState(err_, true);
    } else if (IsEnabled(portc, 6) && !IsEnabled(portb, 6)) {
      SetLedState(status_, true);
    }

    // The wiring of the LED matrix for threeboard v1 is detailed below.
    // Scanning is row-major, meaning only one row is active at a time. The
    // column pins can be effectively considered as active low.
    // row0: PD7, B0_{4,5,6,7}
    // row1: PB4, B0_{0,1,2,3}
    // row2: PD6, B1_{4,5,6,7}
    // row3: PD4, B1_{0,1,2,3}
    // row4: PB5, (R, G, B, PROG)
    // col0: PF0, (R, B0_3, B0_7, B1_3, B1_7)
    // col1: PF1, (G, B0_2, B0_6, B1_2, B1_6)
    // col2: PF4, (B, B0_1, B0_5, B1_1, B1_5)
    // col3: PF5, (PROG, B0_0, B0_4, B1_0, B1_4)
    if (IsEnabled(portd, 7)) {
      SetLedState(bank0_[7], !IsEnabled(portf, 0));
      SetLedState(bank0_[6], !IsEnabled(portf, 1));
      SetLedState(bank0_[5], !IsEnabled(portf, 4));
      SetLedState(bank0_[4], !IsEnabled(portf, 5));
    } else if (IsEnabled(portb, 4)) {
      SetLedState(bank0_[3], !IsEnabled(portf, 0));
      SetLedState(bank0_[2], !IsEnabled(portf, 1));
      SetLedState(bank0_[1], !IsEnabled(portf, 4));
      SetLedState(bank0_[0], !IsEnabled(portf, 5));
    } else if (IsEnabled(portd, 6)) {
      SetLedState(bank1_[7], !IsEnabled(portf, 0));
      SetLedState(bank1_[6], !IsEnabled(portf, 1));
      SetLedState(bank1_[5], !IsEnabled(portf, 4));
      SetLedState(bank1_[4], !IsEnabled(portf, 5));
    } else if (IsEnabled(portd, 4)) {
      SetLedState(bank1_[3], !IsEnabled(portf, 0));
      SetLedState(bank1_[2], !IsEnabled(portf, 1));
      SetLedState(bank1_[1], !IsEnabled(portf, 4));
      SetLedState(bank1_[0], !IsEnabled(portf, 5));
    } else if (IsEnabled(portb, 5)) {
      SetLedState(r_, !IsEnabled(portf, 0));
      SetLedState(g_, !IsEnabled(portf, 1));
      SetLedState(b_, !IsEnabled(portf, 4));
      SetLedState(prog_, !IsEnabled(portf, 5));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

}  // namespace simulator
}  // namespace threeboard