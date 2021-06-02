#pragma once

#include "src/delegates/layer_controller_delegate.h"
#include "src/layers/default_layer.h"
#include "src/layers/layer_b.h"
#include "src/layers/layer_g.h"
#include "src/layers/layer_r.h"
#include "src/led_state.h"
#include "src/storage/storage_controller.h"
#include "src/usb/usb_controller.h"

namespace threeboard {

class LayerController final : public LayerControllerDelegate {
 public:
  LayerController(LedState *led_state, usb::UsbController *usb_controller,
                  storage::StorageController *storage_controller);

  void HandleEvent(const Keypress &);

 private:
  // Implement the LayerControllerDelegate overrides.
  void SwitchToLayer(const LayerId &) override;

  // Define all of the concrete layers of the threeboard. They need to be held
  // in memory here but are only accessed via the layer array.
  DefaultLayer layer_default_;
  LayerR layer_r_;
  LayerG layer_g_;
  LayerB layer_b_;

  // Current layer of the keyboard.
  LayerId current_layer_;
  Layer *layer_[4]{};
};
}  // namespace threeboard