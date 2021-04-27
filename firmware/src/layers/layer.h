#pragma once

#include "src/delegates/event_handler_delegate.h"
#include "src/layers/layer_id.h"
#include "src/led_state.h"
#include "src/usb/usb_controller.h"

namespace threeboard {

// This interface defines the methods used for interacting with a specific layer
// of the threeboard.
class Layer {
 public:
  virtual ~Layer() = default;

  explicit Layer(LedState *led_state, usb::UsbController *usb_controller)
      : led_state_(led_state), usb_controller_(usb_controller) {}

  // Handle a keypress event.
  virtual void HandleEvent(const Keypress &) = 0;

  // Called when the threeboard has transitioned to this layer.
  virtual void TransitionedToLayer() = 0;

 protected:
  virtual void FlushToHost();

  void UpdateLedState(LayerId layer_id);

  LedState *led_state_;
  usb::UsbController *usb_controller_;

  uint8_t bank0_ = 0;
  uint8_t bank1_ = 0;
};
}  // namespace threeboard