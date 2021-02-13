#pragma once

#include "src/event_buffer.h"
#include "src/key_controller.h"
#include "src/layers/layer_controller.h"
#include "src/led_controller.h"
#include "src/native/native.h"
#include "src/usb/usb.h"

namespace threeboard {

// Manages the state of the keyboard and acts as a delegate to coordinate all of
// the various timer interrupt driven handlers.
class Threeboard : public TimerInterruptHandlerDelegate {
public:
  Threeboard(native::Native *native, usb::Usb *usb, EventBuffer *event_buffer,
             LedController *led_controller, KeyController *key_controller);
  ~Threeboard() override = default;

  // Main application event loop.
  void RunEventLoop();

  // Implement the InterruptHandlerDelegate overrides. Timer1 is used to provide
  // a clock signal to the LedController, and Timer2 is used to provide a clock
  // signal to the KeyController.
  void HandleTimer1Interrupt() override;
  void HandleTimer3Interrupt() override;

private:
  // All of the components composed into this class which we need to coordinate.
  native::Native *native_;
  usb::Usb *usb_;
  EventBuffer *event_buffer_;
  LedController *led_controller_;
  KeyController *key_controller_;

  LayerController layer_controller_;
};

} // namespace threeboard
