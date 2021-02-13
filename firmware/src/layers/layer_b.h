#pragma once

#include "src/delegates/layer_controller_delegate.h"
#include "src/layers/layer.h"

namespace threeboard {

class LayerB : public Layer {
public:
  LayerB(LedController *led_controller, usb::Usb *usb,
         LayerControllerDelegate *layer_controller_delegate)
      : Layer(led_controller, usb),
        layer_controller_delegate_(layer_controller_delegate) {}

  void HandleEvent(const Keypress &) override;

  // Called when the threeboard has transitioned to this layer.
  void TransitionedToLayer() override;

private:
  LayerControllerDelegate *layer_controller_delegate_;
};

} // namespace threeboard
