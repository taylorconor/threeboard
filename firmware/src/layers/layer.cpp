#include "src/layers/layer.h"

namespace threeboard {

using LedState = LedController::LedState;

void Layer::UpdateLedState(LayerId layer_id) {
  switch (layer_id) {
    case LayerId::DFLT:
      led_controller_->SetR(LedState::OFF);
      led_controller_->SetG(LedState::OFF);
      led_controller_->SetB(LedState::OFF);
      break;
    case LayerId::R:
      led_controller_->SetR(LedState::ON);
      led_controller_->SetG(LedState::OFF);
      led_controller_->SetB(LedState::OFF);
      break;
    case LayerId::G:
      led_controller_->SetR(LedState::OFF);
      led_controller_->SetG(LedState::ON);
      led_controller_->SetB(LedState::OFF);
      break;
    case LayerId::B:
      led_controller_->SetR(LedState::OFF);
      led_controller_->SetG(LedState::OFF);
      led_controller_->SetB(LedState::ON);
      break;
  }
  led_controller_->SetBank0(bank0_);
  led_controller_->SetBank1(bank1_);
}

void Layer::FlushToHost() { usb_->SendKeypress(bank0_, bank1_); }

}  // namespace threeboard