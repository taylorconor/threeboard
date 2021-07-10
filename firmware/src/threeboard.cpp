#include "src/threeboard.h"

#include "src/logging.h"

#if (defined(AVR) && (!defined(__GNUC__) || __GNUC__ < 9))
static_assert(false, "Unsupported compiler: threeboard requires avr-gcc >=9");
#endif

namespace threeboard {

Threeboard::Threeboard(native::Native *native, EventBuffer *event_buffer,
                       usb::UsbController *usb_controller,
                       storage::StorageController *storage_controller,
                       LedController *led_controller,
                       KeyController *key_controller)
    : native_(native),
      event_buffer_(event_buffer),
      usb_controller_(usb_controller),
      storage_controller_(storage_controller),
      led_controller_(led_controller),
      key_controller_(key_controller),
      layer_controller_(led_controller_->GetLedState(), usb_controller_,
                        storage_controller_) {
  native_->SetTimerInterruptHandlerDelegate(this);
  native_->EnableTimer1();
  native_->EnableTimer3();
}

void Threeboard::RunEventLoop() {
  native_->EnableInterrupts();

  // Busy loop until USB setup succeeds.
  WaitForUsbSetup();

  // Busy loop until USB configuration succeeds.
  WaitForUsbConfiguration();

  // Initialize the storage controller, handle initialization errors and retry
  // as necessary.
  InitializeStorageController();

  // Display the "boot indicator" (the lighting of LEDs R, G and then B in
  // sequence) to show that the threeboard has booted. This method blocks until
  // the boot indicator sequence has finished (250ms) so it doesn't overwrite
  // any LED states that may be set in the event loop.
  DisplayBootIndicator();

  // Main event loop.
  while (true) {
    RunEventLoopIteration();
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

  if (boot_indicator_state_.status > 0) {
    PollBootIndicator();
  }
}

void Threeboard::WaitForUsbSetup() {
  while (!usb_controller_->Setup()) {
    led_controller_->GetLedState()->SetErr(LedState::BLINK);
    // This is an unrecoverable error. We can either crash here, or delay before
    // retrying USB setup from scratch repeatedly in the hopes that setup
    // eventually succeeds. We choose not to crash.
    native_->DelayMs(50);
  }
  led_controller_->GetLedState()->SetErr(LedState::OFF);
}

void Threeboard::WaitForUsbConfiguration() {
  // Fast busy loop until the USB stack configures. If this never happens it
  // will continue to loop infinitely, but also blink the error LED.
  uint8_t iterations = 0;
  while (!usb_controller_->HasConfigured()) {
    iterations += 1;
    // After 2.5 seconds, begin flashing the ERR LED.
    if (iterations > 250) {
      LOG_ONCE("Failed to configure USB, continuing to retry");
      led_controller_->GetLedState()->SetErr(LedState::BLINK);
    }
    native_->DelayMs(10);
  }
  led_controller_->GetLedState()->SetErr(LedState::OFF);
}

void Threeboard::InitializeStorageController() {
  bool status = storage_controller_->InitializeManifest();
  // This is likely fatal. Blink both ERR and STATUS LEDs (to distinguish from a
  // USB failure where only the ERR LED blinks) and keep retrying.
  while (!status) {
    led_controller_->GetLedState()->SetErr(LedState::BLINK);
    led_controller_->GetLedState()->SetStatus(LedState::BLINK);
    status = storage_controller_->InitializeManifest();
  }
}

void Threeboard::DisplayBootIndicator() {
  boot_indicator_state_.status = 1;
  native_->DelayMs(255);
}

void Threeboard::PollBootIndicator() {
  boot_indicator_state_.counter += 1;

  // This method is polled every 5ms. Each LED is lit for 80ms in sequence.
  if (boot_indicator_state_.counter > 16) {
    boot_indicator_state_.counter = 0;
    boot_indicator_state_.status += 1;
    if (boot_indicator_state_.status > 4) {
      boot_indicator_state_.status = 0;
      led_controller_->GetLedState()->SetB(LedState::OFF);
    } else {
      if (boot_indicator_state_.status == 2) {
        led_controller_->GetLedState()->SetR(LedState::ON);
      } else if (boot_indicator_state_.status == 3) {
        led_controller_->GetLedState()->SetR(LedState::OFF);
        led_controller_->GetLedState()->SetG(LedState::ON);
      } else if (boot_indicator_state_.status == 4) {
        led_controller_->GetLedState()->SetG(LedState::OFF);
        led_controller_->GetLedState()->SetB(LedState::ON);
      }
    }
  }
}

void Threeboard::RunEventLoopIteration() {
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
      // TODO: surface an error here if event handling fails.
      /*bool status =*/
      layer_controller_.HandleEvent(event_buffer_->GetKeypressEvent());
    }
    // Re-enable interrupts after handling the event.
    native_->EnableInterrupts();
  }
}

}  // namespace threeboard
