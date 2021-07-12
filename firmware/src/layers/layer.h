#pragma once

#include "src/delegates/event_handler_delegate.h"
#include "src/layers/layer_id.h"
#include "src/led_state.h"
#include "src/usb/usb_controller.h"
#include "src/util/util.h"

namespace threeboard {

// This interface defines the methods used for interacting with a specific layer
// of the threeboard.
class Layer {
 public:
  virtual ~Layer() = default;

  Layer(LedState *led_state, usb::UsbController *usb_controller)
      : led_state_(led_state), usb_controller_(usb_controller) {}

  // Handle a keypress event.
  virtual bool HandleEvent(const Keypress &) = 0;

  // Called when the threeboard has transitioned to this layer.
  virtual void TransitionedToLayer() = 0;

 protected:
  virtual void SendToHost(uint8_t key, uint8_t mod);

  void UpdateLedState(LayerId layer_id, uint8_t bank0, uint8_t bank1);

  LedState *led_state_;
  usb::UsbController *usb_controller_;

  // True if this layer is currently in program mode.
  bool prog_ = false;
};
}  // namespace threeboard