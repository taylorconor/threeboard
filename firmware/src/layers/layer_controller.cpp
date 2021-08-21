#include "src/layers/layer_controller.h"

namespace threeboard {

LayerController::LayerController(LedState *led_state,
                                 usb::UsbController *usb_controller,
                                 storage::StorageController *storage_controller)
    : layer_default_(led_state, usb_controller, this),
      layer_r_(led_state, usb_controller, storage_controller, this),
      layer_g_(led_state, usb_controller, storage_controller, this),
      layer_b_(led_state, usb_controller, storage_controller, this) {
  // Associate each Layer reference to the corresponding LayerId.
  layer_[LayerId::DFLT] = &layer_default_;
  layer_[LayerId::R] = &layer_r_;
  layer_[LayerId::G] = &layer_g_;
  layer_[LayerId::B] = &layer_b_;
}

// Test-only constructor.
LayerController::LayerController(Layer *layer_dflt, Layer *layer_r,
                                 Layer *layer_g, Layer *layer_b)
    : layer_default_(nullptr, nullptr, nullptr),
      layer_r_(nullptr, nullptr, nullptr, nullptr),
      layer_g_(nullptr, nullptr, nullptr, nullptr),
      layer_b_(nullptr, nullptr, nullptr, nullptr) {
  layer_[LayerId::DFLT] = layer_dflt;
  layer_[LayerId::R] = layer_r;
  layer_[LayerId::G] = layer_g;
  layer_[LayerId::B] = layer_b;
}

bool LayerController::HandleEvent(const threeboard::Keypress &keypress) {
  return layer_[current_layer_]->HandleEvent(keypress);
}

bool LayerController::SwitchToLayer(const LayerId &layer_id) {
  current_layer_ = layer_id;
  return layer_[current_layer_]->TransitionedToLayer();
}

}  // namespace threeboard