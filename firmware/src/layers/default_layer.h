#pragma once

#include "src/delegates/layer_controller_delegate.h"
#include "src/layers/layer.h"

namespace threeboard {

class DefaultLayer : public Layer {
 public:
  DefaultLayer(LedState *led_state, usb::UsbController *usb,
               LayerControllerDelegate *layer_controller_delegate)
      : Layer(led_state, usb),
        layer_controller_delegate_(layer_controller_delegate) {}

  void HandleEvent(const Keypress &) override;

  // Called when the threeboard has transitioned to this layer.
  void TransitionedToLayer() override;

 private:
  LayerControllerDelegate *layer_controller_delegate_;
};

}  // namespace threeboard
