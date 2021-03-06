#include "src/threeboard.h"

#include "src/logging.h"

#if (defined(AVR) && (!defined(__GNUC__) || __GNUC__ < 9))
static_assert(false, "Unsupported compiler: threeboard requires avr-gcc >=9");
#endif

namespace threeboard {

Threeboard::Threeboard(native::Native *native, usb::Usb *usb,
                       EventBuffer *event_buffer, LedController *led_controller,
                       KeyController *key_controller)
    : native_(native),
      usb_(usb),
      event_buffer_(event_buffer),
      led_controller_(led_controller),
      key_controller_(key_controller),
      layer_controller_(led_controller_->GetLedState(), usb_) {
  // TODO: provide `this` as delegate to receive callback for success/error?
  usb_->Setup();
  native_->SetTimerInterruptHandlerDelegate(this);
  native_->EnableTimer1();
  native_->EnableTimer3();
}

void Threeboard::RunEventLoop() {
  native_->EnableInterrupts();

  // Wait until the USB stack has been configured before continuing the event
  // loop.
  // TODO: add error handling.
  while (!usb_->HasConfigured())
    ;

  // Main event loop.
  while (true) {
    // Atomically check for new keyboard events, and either handle them or
    // sleep the CPU until the next interrupt.
    native_->DisableInterrupts();
    if (!event_buffer_->HasEvent()) {
      // Sleep the CPU until another interrupt fires.
      native_->EnableCpuSleep();
      native_->EnableInterrupts();
      native_->SleepCpu();
      native_->DisableCpuSleep();
    } else {
      if (event_buffer_->HasKeypressEvent()) {
        layer_controller_.HandleEvent(event_buffer_->GetKeypressEvent());
      }
      // Re-enable interrupts after handling the event.
      native_->EnableInterrupts();
    }
  }
}

void Threeboard::HandleTimer1Interrupt() {
  LOG_ONCE("Timer 1 setup complete");
  led_controller_->ScanNextLine();
}

void Threeboard::HandleTimer3Interrupt() {
  LOG_ONCE("Timer 3 setup complete");
  key_controller_->PollKeyState();
  led_controller_->UpdateBlinkStatus();
}

}  // namespace threeboard
