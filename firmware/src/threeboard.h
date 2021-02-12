#pragma once

#include "src/event_buffer.h"
#include "src/key_controller.h"
#include "src/layers/default_layer.h"
#include "src/layers/layer.h"
#include "src/layers/layer_b.h"
#include "src/layers/layer_g.h"
#include "src/layers/layer_id.h"
#include "src/layers/layer_r.h"
#include "src/led_controller.h"
#include "src/native/native.h"
#include "src/usb/usb.h"

// Manages the state of the keyboard and acts as a delegate to coordinate all of
// the various timer interrupt driven handlers.
namespace threeboard {
class Threeboard : public TimerInterruptHandlerDelegate,
                   public LayerControllerDelegate {
public:
  Threeboard(native::Native *native, usb::Usb *usb, EventBuffer *event_buffer,
             LedController *led_controller, KeyController *key_controller);
  ~Threeboard() override = default;

  // Main application event loop.
  void Run();

  // Implement the InterruptHandlerDelegate overrides. Timer1 is used to provide
  // a clock signal to the LedController, and Timer2 is used to provide a clock
  // signal to the KeyController.
  void HandleTimer1Interrupt() override;
  void HandleTimer3Interrupt() override;

  // Implement the LayerControllerDelegate overrides.
  void SwitchToLayer(const LayerId &layer_id) override;
  void FlushToHost() override;

private:
  // All of the components composed into this class which we need to coordinate.
  native::Native *native_;
  usb::Usb *usb_;
  EventBuffer *event_buffer_;
  LedController *led_controller_;
  KeyController *key_controller_;

  // Define all of the concrete layers of the threeboard. They need to be held
  // in memory here but are only accessed via the layer array.
  DefaultLayer layer_default_;
  LayerR layer_r_;
  LayerG layer_g_;
  LayerB layer_b_;

  // Current layer of the keyboard.
  LayerId layer_id_;
  Layer *layer_[4]{};
};
} // namespace threeboard
