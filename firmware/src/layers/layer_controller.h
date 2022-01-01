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

// A class to abstract away interactions with specific layers. This class keeps
// track of the current layer and passes keypress events to it.
class LayerController : public LayerControllerDelegate {
 public:
  LayerController(LedState *led_state, usb::UsbController *usb_controller,
                  storage::StorageController *storage_controller);

  virtual bool HandleEvent(const Keypress &);
  bool SwitchToLayer(const LayerId &) override;

 protected:
  // Test-only
  LayerController(Layer *layer_dflt, Layer *layer_r, Layer *layer_g,
                  Layer *layer_b);

 private:
  // Define all of the concrete layers of the threeboard. They need to be held
  // in memory here but are only accessed via the layer array.
  DefaultLayer layer_default_;
  LayerR layer_r_;
  LayerG layer_g_;
  LayerB layer_b_;

  // Current layer of the keyboard.
  LayerId current_layer_ = LayerId::DFLT;
  Layer *layer_[4]{};
};
}  // namespace threeboard