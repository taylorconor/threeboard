#include "threeboard.h"

#include "util/enums.h"

#if !(defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9)
static_assert(false, "Unsupported compiler. Threeboard requires clang (>=5) or "
                     "gcc/avr-gcc (>=9)");
#endif

namespace threeboard {
namespace {
using LedState = LedController::LedState;
}

const Threeboard::handler_function Threeboard::state_machine[4][2][1] = {
    [Layer::DEFAULT] = {[State::INPUT] = {&Threeboard::HandleDefaultInput},
                        [State::FLUSH] = {&Threeboard::HandleDefaultFlush}},
    [Layer::R] = {[State::INPUT] = {&Threeboard::HandleDefaultInput},
                  [State::FLUSH] = {&Threeboard::HandleDefaultFlush}},
    [Layer::G] = {[State::INPUT] = {&Threeboard::HandleDefaultInput},
                  [State::FLUSH] = {&Threeboard::HandleDefaultFlush}},
    [Layer::B] = {[State::INPUT] = {&Threeboard::HandleDefaultInput},
                  [State::FLUSH] = {&Threeboard::HandleDefaultFlush}}};

Threeboard::Threeboard(native::Native *native, EventHandler *event_handler,
                       LedController *led_controller,
                       KeyController *key_controller)
    : native_(native), event_handler_(event_handler),
      led_controller_(led_controller), key_controller_(key_controller) {
  native_->SetTimer1InterruptHandler(this);
  native_->SetTimer3InterruptHandler(this);

  // Set up initial layers and states. These can be restored from flash later if
  // we decide to preserve them.
  layer_ = Layer::DEFAULT;
}

void Threeboard::Run() {
  while (1) {
    auto event = event_handler_->WaitForKeyboardEvent();
    auto state = properties_[layer_].state;
    (this->*state_machine[layer_][state][0])(event);
    UpdateLedState();
  }
}

void Threeboard::UpdateLedState() {
  switch (layer_) {
  case Layer::DEFAULT:
    led_controller_->SetR(LedState::OFF);
    led_controller_->SetG(LedState::OFF);
    led_controller_->SetB(LedState::OFF);
    break;
  case Layer::R:
    led_controller_->SetR(LedState::ON);
    led_controller_->SetG(LedState::OFF);
    led_controller_->SetB(LedState::OFF);
    break;
  case Layer::G:
    led_controller_->SetR(LedState::OFF);
    led_controller_->SetG(LedState::ON);
    led_controller_->SetB(LedState::OFF);
    break;
  case Layer::B:
    led_controller_->SetR(LedState::OFF);
    led_controller_->SetG(LedState::OFF);
    led_controller_->SetB(LedState::ON);
    break;
  }
  led_controller_->SetBank0(properties_[layer_].bank0);
  led_controller_->SetBank1(properties_[layer_].bank1);
}

void Threeboard::SwitchToNextLayer() {
  layer_ = (Layer)(((uint8_t)layer_ + 1) % util::element_count<Layer>());
}

void Threeboard::HandleDefaultInput(const Keypress keypress) {
  if (keypress == Keypress::X) {
    properties_[layer_].bank0++;
  } else if (keypress == Keypress::Y) {
    properties_[layer_].bank1++;
  } else if (keypress == Keypress::Z) {
    properties_[layer_].bank0 = 0;
    properties_[layer_].bank1 = 0;
  } else if (keypress == Keypress::XYZ) {
    SwitchToNextLayer();
  }
}

void Threeboard::HandleDefaultFlush(const Keypress keypress) {}

void Threeboard::HandleTimer1Interrupt() { led_controller_->ScanNextLine(); }

void Threeboard::HandleTimer3Interrupt() { key_controller_->PollKeyState(); }

} // namespace threeboard
