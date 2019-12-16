#pragma once

#include "key_controller.h"
#include "led_controller.h"
#include "native/native.h"

// Manages the state of the keyboard and acts as a delegate to coordinate all of
// the various interrupt-driven handlers.
namespace threeboard {
class Threeboard : public KeypressHandlerDelegate,
                   public native::InterruptHandlerDelegate {
public:
  Threeboard(native::Native *native);

  // Main application runloop.
  void Run();

  // Implement the KeypressHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos.
  void HandleKeypress(const uint8_t) override;

  // Implement the InterruptHandlerDelegate overrides. Timer1 is used to clock
  // the LedController, and Timer2 is used to clock the KeyController.
  void HandleTimer1Interrupt() override;
  void HandleTimer3Interrupt() override;

private:
  native::Native *native_;
  LedController led_controller_;
  KeyController key_controller_;
};
} // namespace threeboard
