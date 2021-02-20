#pragma once

#include "src/delegates/keypress_handler_delegate.h"
#include "src/layers/layer_id.h"
#include "src/led_controller.h"
#include "src/usb/usb.h"

namespace threeboard {

// This interface defines the methods used for interacting with a specific layer
// of the threeboard.
class Layer {
 public:
  virtual ~Layer() = default;

  explicit Layer(LedController *led_controller, usb::Usb *usb)
      : led_controller_(led_controller), usb_(usb) {}

  // Handle a keypress event.
  virtual void HandleEvent(const Keypress &) = 0;

  // Called when the threeboard has transitioned to this layer.
  virtual void TransitionedToLayer() = 0;

 protected:
  virtual void FlushToHost();

  void UpdateLedState(LayerId layer_id);

  LedController *led_controller_;
  usb::Usb *usb_;

  uint8_t bank0_ = 0;
  uint8_t bank1_ = 0;
};
}  // namespace threeboard