#pragma once

#include "event_handler.h"
#include "key_controller.h"
#include "led_controller.h"
#include "native/native.h"
#include "native/usb.h"

// Manages the state of the keyboard and acts as a delegate to coordinate all of
// the various interrupt-driven handlers.
namespace threeboard {
class Threeboard : public native::InterruptHandlerDelegate {
public:
  Threeboard(native::Native *native, native::Usb *usb,
             EventHandler *event_handler, LedController *led_controller,
             KeyController *key_controller);

  // Main application runloop.
  void Run();

  // Implement the InterruptHandlerDelegate overrides. Timer1 is used to clock
  // the LedController, and Timer2 is used to clock the KeyController.
  void HandleTimer1Interrupt() override;
  void HandleTimer3Interrupt() override;

  // Methods for handling events in the state machine.
  void HandleDefaultInput(const Keypress);
  void HandleDefaultFlush(const Keypress);

private:
  // All possible layers in the threeboard.
  enum Layer : uint8_t {
    DEFAULT = 0,
    R = 1,
    G = 2,
    B = 3,
  };

  // All possible states in the threeboard state machine.
  enum State : uint8_t {
    INPUT = 0,
    FLUSH = 1,
  };

  // The per-layer properties of the threeboard. These are akways common
  // properties for each layer (since each layer has access to the same LEDs
  // etc). It also stores the current state at each layer, which allows the
  // threeboard to restore the correct previous state when switching layers.
  struct LayerProperties {
    uint8_t bank0 = 0;
    uint8_t bank1 = 0;
    State state = State::INPUT;
  };

  // The state machine maps a handler function to a specific state within a
  // layer. Handler functions are not generally state-specific, so they can be
  // reused.
  typedef void (Threeboard::*handler_function)(const Keypress);
  static const handler_function state_machine[4][2][1];

  native::Native *native_;
  native::Usb *usb_;
  EventHandler *event_handler_;
  LedController *led_controller_;
  KeyController *key_controller_;
  Layer layer_;
  LayerProperties properties_[4];

  void UpdateLedState();
  void SwitchToNextLayer();
};
} // namespace threeboard
