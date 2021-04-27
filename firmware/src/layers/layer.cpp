#include "src/layers/layer.h"

#include "src/logging.h"

namespace threeboard {

void Layer::UpdateLedState(LayerId layer_id) {
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
  led_state_->SetBank0(bank0_);
  led_state_->SetBank1(bank1_);
}

void Layer::FlushToHost() {
  bool success = usb_controller_->SendKeypress(bank0_, bank1_);
  if (!success) {
    LOG("Failed to flush to host");
    led_state_->SetErr(LedState::ON);
  } else {
    led_state_->SetErr(LedState::OFF);
  }
}

}  // namespace threeboard