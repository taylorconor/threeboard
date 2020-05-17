#include "sim_runner.h"

#include "sim_ui.h"
#include <iostream>

namespace threeboard {
namespace simulator {
namespace {
using namespace std::placeholders;

// Used to test a single pin in a register.
bool IsEnabled(uint8_t reg, uint8_t pin) { return reg & (1 << pin); }

} // namespace

SimRunner::SimRunner() { simulator_ = std::make_unique<Simulator>(); }

void SimRunner::RunSimulator() {
  if (sim_ui_ != nullptr) {
    std::cout << "Attempted to run a running simulator instance!" << std::endl;
    exit(0);
  }
  simulator_->Reset();
  simulator_->RunAsync();
  sim_ui_ = std::make_unique<SimUI>(
      std::bind(&SimRunner::UpdateSimState, this),
      std::bind(&SimRunner::HandleKeypress, this, _1, _2),
      simulator_->GetState(), simulator_->GetCycleCount());
  sim_ui_->StartRenderLoopAsync();
  std::unique_lock<std::mutex> lock(mutex_);
  cond_var_.wait(lock);
  sim_ui_ = nullptr;
}

void SimRunner::UpdateSimState() {
  uint8_t portb = simulator_->GetPortB();
  uint8_t portc = simulator_->GetPortC();
  uint8_t portd = simulator_->GetPortD();
  uint8_t portf = simulator_->GetPortF();

  // Clear the LED state before the next frame is calculated.
  sim_ui_->ClearLedState();

  // The ERR/STATUS LED pair for threeboard v1 is wired as follows:
  // status0: PB6, ERR
  // status1: PC6, STATUS
  if (IsEnabled(portb, 6) && !IsEnabled(portc, 6)) {
    sim_ui_->SetErr(true);
  } else if (IsEnabled(portc, 6) && !IsEnabled(portb, 6)) {
    sim_ui_->SetStatus(true);
  }

  // The wiring of the LED matrix for threeboard v1 is detailed below. Scanning
  // is row-major, meaning only one row is active at a time. The column pins
  // can be effectively considered as active low.
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
    sim_ui_->SetBank0(!IsEnabled(portf, 0), 7);
    sim_ui_->SetBank0(!IsEnabled(portf, 1), 6);
    sim_ui_->SetBank0(!IsEnabled(portf, 4), 5);
    sim_ui_->SetBank0(!IsEnabled(portf, 5), 4);
  } else if (IsEnabled(portb, 4)) {
    sim_ui_->SetBank0(!IsEnabled(portf, 0), 3);
    sim_ui_->SetBank0(!IsEnabled(portf, 1), 2);
    sim_ui_->SetBank0(!IsEnabled(portf, 4), 1);
    sim_ui_->SetBank0(!IsEnabled(portf, 5), 0);
  } else if (IsEnabled(portd, 6)) {
    sim_ui_->SetBank1(!IsEnabled(portf, 0), 7);
    sim_ui_->SetBank1(!IsEnabled(portf, 1), 6);
    sim_ui_->SetBank1(!IsEnabled(portf, 4), 5);
    sim_ui_->SetBank1(!IsEnabled(portf, 5), 4);
  } else if (IsEnabled(portd, 4)) {
    sim_ui_->SetBank1(!IsEnabled(portf, 0), 3);
    sim_ui_->SetBank1(!IsEnabled(portf, 1), 2);
    sim_ui_->SetBank1(!IsEnabled(portf, 4), 1);
    sim_ui_->SetBank1(!IsEnabled(portf, 5), 0);
  } else if (IsEnabled(portb, 5)) {
    sim_ui_->SetR(!IsEnabled(portf, 0));
    sim_ui_->SetG(!IsEnabled(portf, 1));
    sim_ui_->SetB(!IsEnabled(portf, 4));
    sim_ui_->SetProg(!IsEnabled(portf, 5));
  }
  return;
}

void SimRunner::HandleKeypress(SimUI::Key key, bool state) {
  if (key == SimUI::Key::KEY_Q) {
    cond_var_.notify_all();
    return;
  }

  // The key pins are all active low.
  if (key == SimUI::Key::KEY_A) {
    // Switch 1 - maps to PB2.
    simulator_->SetPinB(2, !state);
  } else if (key == SimUI::Key::KEY_S) {
    // Switch 2 - maps to PB3.
    simulator_->SetPinB(3, !state);
  } else if (key == SimUI::Key::KEY_D) {
    // Switch 3 - maps to PB1.
    simulator_->SetPinB(1, !state);
  }
}
} // namespace simulator
} // namespace threeboard
