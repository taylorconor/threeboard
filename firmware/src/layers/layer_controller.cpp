#include "src/layers/layer_controller.h"

namespace threeboard {

LayerController::LayerController(LedController *led_controller, usb::Usb *usb)
    : layer_default_(led_controller, usb, this),
      layer_r_(led_controller, usb, this),
      layer_g_(led_controller, usb, this),
      layer_b_(led_controller, usb, this),
      current_layer_(LayerId::DFLT) {
  // Associate each Layer reference to the corresponding LayerId.
  layer_[LayerId::DFLT] = &layer_default_;
  layer_[LayerId::R] = &layer_r_;
  layer_[LayerId::G] = &layer_g_;
  layer_[LayerId::B] = &layer_b_;
}

void LayerController::HandleEvent(const threeboard::Keypress &keypress) {
  layer_[current_layer_]->HandleEvent(keypress);
}

void LayerController::SwitchToLayer(const LayerId &layer_id) {
  current_layer_ = layer_id;
  layer_[current_layer_]->TransitionedToLayer();
}

}  // namespace threeboard