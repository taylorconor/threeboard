#pragma once

#include "src/delegates/error_handler_delegate.h"
#include "src/event_buffer.h"
#include "src/key_controller.h"
#include "src/layers/layer_controller.h"
#include "src/led_controller.h"
#include "src/native/native.h"
#include "src/usb/usb.h"

namespace threeboard {

// Manages the state of the keyboard and acts as a delegate to coordinate all of
// the various timer interrupt driven and error-driven handlers.
class Threeboard : public TimerInterruptHandlerDelegate,
                   public ErrorHandlerDelegate {
 public:
  Threeboard(native::Native *native, usb::Usb *usb, EventBuffer *event_buffer,
             LedController *led_controller, KeyController *key_controller);
  ~Threeboard() override = default;

  // Main application event loop.
  void RunEventLoop();

  // Implement the InterruptHandlerDelegate overrides. Timer1 is used to provide
  // a clock signal to the LedController, and Timer2 is used to provide a clock
  // signal to the KeyController.
  void HandleTimer1Interrupt() final;
  void HandleTimer3Interrupt() final;

 private:
  // All of the components composed into this class which we need to coordinate.
  native::Native *native_;
  usb::Usb *usb_;
  EventBuffer *event_buffer_;
  LedController *led_controller_;
  KeyController *key_controller_;

  LayerController layer_controller_;

  // Because RunEventLoop() is an infinite loop, it's not fully testable.
  // Instead the main parts of the event loop are broken out into smaller
  // functions with no infinite loop. These functions are internal to the
  // Threeboard class and shouldn't be exposed publicly, so to test them
  // properly we declare a friend relationship with the ThreeboardTest fixture.
  friend class ThreeboardTest;

  void WaitForUsbSetup();
  void WaitForUsbConfiguration();
  void RunEventLoopIteration();
};

}  // namespace threeboard
