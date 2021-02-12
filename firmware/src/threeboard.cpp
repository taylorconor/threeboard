#include "threeboard.h"

#include "src/logging.h"

#if (defined(AVR) && (!defined(__GNUC__) || __GNUC__ < 9))
static_assert(false, "Unsupported compiler: threeboard requires avr-gcc >=9");
#endif

namespace threeboard {

using LedState = LedController::LedState;

Threeboard::Threeboard(native::Native *native, usb::Usb *usb,
                       EventBuffer *event_buffer, LedController *led_controller,
                       KeyController *key_controller)
    : native_(native), usb_(usb), event_buffer_(event_buffer),
      led_controller_(led_controller), key_controller_(key_controller),
      layer_default_(led_controller_, this), layer_r_(led_controller_, this),
      layer_g_(led_controller_, this), layer_b_(led_controller_, this) {
  // TODO: provide `this` as delegate to receive callback for success/error?
  usb_->Setup();
  native_->SetTimerInterruptHandlerDelegate(this);
  native_->EnableTimer1();
  native_->EnableTimer3();

  // Set up initial layers and states. These can be restored from flash later if
  // we decide to preserve them.
  layer_id_ = LayerId::DFLT;

  // Associate each Layer reference to the corresponding LayerId.
  layer_[LayerId::DFLT] = &layer_default_;
  layer_[LayerId::R] = &layer_r_;
  layer_[LayerId::G] = &layer_g_;
  layer_[LayerId::B] = &layer_b_;
}

void Threeboard::Run() {
  native_->EnableInterrupts();

  // Wait until the USB stack has been configured before continuing the runloop.
  // TODO: add error handling.
  while (!usb_->HasConfigured())
    ;

  // Main event loop.
  while (true) {
    // Atomically check for new keyboard events, and either handle them or
    // sleep the CPU until the next interrupt.
    native_->DisableInterrupts();
    auto event = event_buffer_->GetPendingEventIfAvailable();
    if (event == Keypress::INACTIVE) {
      // Sleep the CPU until another interrupt fires.
      native_->EnableCpuSleep();
      native_->EnableInterrupts();
      native_->SleepCpu();
      native_->DisableCpuSleep();
    } else {
      // Re-enable interrupts and handle the keyboard event.
      native_->EnableInterrupts();
      layer_[layer_id_]->HandleEvent(event);
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

void Threeboard::SwitchToLayer(const LayerId &layer_id) {
  layer_id_ = layer_id;
  layer_[layer_id_]->TransitionedToLayer();
}

void Threeboard::FlushToHost() {
  // TODO: this will need to be moved down to the relevant Layer implementations
  // once more advanced functionality has been implemented.
  usb_->SendKeypress(layer_[layer_id_]->Bank0(), layer_[layer_id_]->Bank1());
}

} // namespace threeboard
