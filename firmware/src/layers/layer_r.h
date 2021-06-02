#pragma once

#include "src/delegates/layer_controller_delegate.h"
#include "src/layers/layer.h"

namespace threeboard {

class LayerR final : public Layer {
 public:
  LayerR(LedState *led_state, usb::UsbController *usb_controller,
         storage::StorageController *storage_controller,
         LayerControllerDelegate *layer_controller_delegate)
      : Layer(led_state, usb_controller, storage_controller),
        layer_controller_delegate_(layer_controller_delegate) {}

  void HandleEvent(const Keypress &) override;

  // Called when the threeboard has transitioned to this layer.
  void TransitionedToLayer() override;

 private:
  LayerControllerDelegate *layer_controller_delegate_;
};

}  // namespace threeboard
