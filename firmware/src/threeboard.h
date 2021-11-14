#pragma once

#include "src/event_buffer.h"
#include "src/key_controller.h"
#include "src/layers/layer_controller.h"
#include "src/led_controller.h"
#include "src/native/native.h"
#include "src/storage/storage_controller.h"
#include "src/usb/usb_controller.h"

namespace threeboard {

// Manages the state of the keyboard and acts as a delegate to coordinate all of
// the various timer interrupt driven handlers.
class Threeboard final : public TimerInterruptHandlerDelegate {
 public:
  Threeboard(native::Native *native, EventBuffer *event_buffer,
             usb::UsbController *usb_controller,
             storage::StorageController *storage_controller,
             LedController *led_controller, KeyController *key_controller,
             LayerController *layer_controller);
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
  EventBuffer *event_buffer_;
  usb::UsbController *usb_controller_;
  storage::StorageController *storage_controller_;
  LedController *led_controller_;
  KeyController *key_controller_;
  LayerController *layer_controller_;

  // A small bit-packed struct to store the state of the LED boot indicator
  // sequence in a single byte.
  struct {
    unsigned status : 3;
    unsigned counter : 5;
  } boot_indicator_state_{0, 0};

  // Because RunEventLoop() is an infinite loop, it's not fully testable.
  // Instead the main parts of the event loop are broken out into smaller
  // functions with no infinite loop. These functions are internal to the
  // Threeboard class and shouldn't be exposed publicly, so to test them
  // properly we declare a friend relationship with the ThreeboardTest fixture.
  friend class ThreeboardTest;

  void WaitForUsbSetup();
  void WaitForUsbConfiguration();
  void DisplayBootIndicator();
  void PollBootIndicator();
  void RunEventLoopIteration();
};

}  // namespace threeboard
