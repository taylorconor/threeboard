#include "threeboard.h"

namespace threeboard {

Threeboard::Threeboard(native::Native *native)
    : native_(native), led_controller_(native_),
      key_controller_(native_, this) {
  native_->SetTimer1InterruptHandler(this);
  native_->SetTimer3InterruptHandler(this);
}

void Threeboard::Run() {
  led_controller_.SetBank1(0xFF);
  while (1) {
    led_controller_.SetR(LedController::State::ON);
    native_->Delay(150);
    led_controller_.SetR(LedController::State::OFF);
    led_controller_.SetG(LedController::State::ON);
    native_->Delay(150);
    led_controller_.SetG(LedController::State::OFF);
    led_controller_.SetB(LedController::State::ON);
    native_->Delay(150);
    led_controller_.SetB(LedController::State::OFF);
    led_controller_.SetProg(LedController::State::ON);
    native_->Delay(150);
    led_controller_.SetProg(LedController::State::OFF);
    led_controller_.SetErr(LedController::State::ON);
    native_->Delay(150);
    led_controller_.SetErr(LedController::State::OFF);
    led_controller_.SetStatus(LedController::State::ON);
    native_->Delay(150);
    led_controller_.SetStatus(LedController::State::OFF);
  }
}

void Threeboard::HandleKeypress(const uint8_t keypress) {
  led_controller_.SetBank0(keypress);
}

void Threeboard::HandleTimer1Interrupt() { led_controller_.ScanNextLine(); }

void Threeboard::HandleTimer3Interrupt() { key_controller_.PollKeyState(); }

} // namespace threeboard
