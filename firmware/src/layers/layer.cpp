#include "src/layers/layer.h"

#include "src/logging.h"

namespace threeboard {

void Layer::UpdateLedState(LayerId layer_id, uint8_t bank0, uint8_t bank1) {
  switch (layer_id) {
    case LayerId::DFLT:
      led_state_->SetR(LedState::OFF);
      led_state_->SetG(LedState::OFF);
      led_state_->SetB(LedState::OFF);
      break;
    case LayerId::R:
      led_state_->SetR(LedState::ON);
      led_state_->SetG(LedState::OFF);
      led_state_->SetB(LedState::OFF);
      break;
    case LayerId::G:
      led_state_->SetR(LedState::OFF);
      led_state_->SetG(LedState::ON);
      led_state_->SetB(LedState::OFF);
      break;
    case LayerId::B:
      led_state_->SetR(LedState::OFF);
      led_state_->SetG(LedState::OFF);
      led_state_->SetB(LedState::ON);
      break;
  }
  led_state_->SetBank0(bank0);
  led_state_->SetBank1(bank1);
  led_state_->SetProg(prog_ ? LedState::ON : LedState::OFF);
}

void Layer::SendToHost(uint8_t key, uint8_t mod) {
  bool success = usb_controller_->SendKeypress(key, mod);
  if (!success) {
    LOG("Failed to send to host");
    led_state_->SetErr(LedState::ON);
  } else {
    led_state_->SetErr(LedState::OFF);
  }
}

}  // namespace threeboard