#include "firmware.h"

#include <iostream>

namespace threeboard {
namespace simulator {

Firmware::Firmware(Simavr *simavr)
    : simavr_(simavr), is_running_(false), should_reset_(false) {}

Firmware::~Firmware() {
  if (is_running_) {
    is_running_ = false;
    if (sim_thread_.joinable()) {
      sim_thread_.join();
    }
    std::cout << "Simulator shut down successfully." << std::endl;
  }
  simavr_->Terminate();
}

void Firmware::RunAsync() {
  // Start a new thread to run the AVR firmware and manage clock timing.
  is_running_ = true;
  sim_thread_ = std::thread(&Firmware::RunDetached, this);
}

void Firmware::Reset() { should_reset_ = true; }

// TODO: assert here if is_running_ is false?
uint8_t Firmware::GetPortB() const { return simavr_->GetData(PORTB); }
uint8_t Firmware::GetPortC() const { return simavr_->GetData(PORTC); }
uint8_t Firmware::GetPortD() const { return simavr_->GetData(PORTD); }
uint8_t Firmware::GetPortF() const { return simavr_->GetData(PORTF); }

void Firmware::SetPinB(uint8_t pin, bool enabled) {
  simavr_->SetData(PINB, (simavr_->GetData(PINB) & ~(1 << pin)));
  if (enabled) {
    simavr_->SetData(PINB, (simavr_->GetData(PINB) | (1 << pin)));
  }
}

int Firmware::GetCpuState() const { return simavr_->GetState(); }

uint64_t Firmware::GetCpuCycleCount() const { return simavr_->GetCycle(); }

bool Firmware::IsGdbEnabled() const { return simavr_->GetGdbPort(); }

uint16_t Firmware::GetDataSectionSize() const {
  return simavr_->GetDataSectionSize();
}

uint16_t Firmware::GetBssSectionSize() const {
  return simavr_->GetBssSectionSize();
}

uint16_t Firmware::GetStackSize() const {
  return simavr_->GetRamSize() - simavr_->GetStackPointer();
}

uint16_t Firmware::GetSramUsage() const {
  // Memory is laid out as follows in the atmega32u4:
  // |- ioports -| |- .data -|- .bss -|- stack >> -----|
  // |-- 255 B --| |-------------- 2.5 K --------------|
  // |--------------- ramsize = 28xx B ----------------|
  double usage = GetDataSectionSize() + GetBssSectionSize() + GetStackSize();
  return (usage / simavr_->GetRamSize()) * 100;
}

void Firmware::EnableGdb(uint16_t port) const {
  simavr_->SetGdbPort(port);
  simavr_->SetState(STOPPED);
  simavr_->InitGdb();
}

void Firmware::DisableGdb() const {
  simavr_->DeinitGdb();
  simavr_->SetState(RUNNING);
  simavr_->SetGdbPort(0);
}

void Firmware::RunDetached() {
  // TODO: it seems that this loop never ends even if the firmware crashes. Why?
  // This is a regression.
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
