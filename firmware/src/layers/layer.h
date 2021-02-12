#pragma once

#include "src/delegates/keypress_handler_delegate.h"
#include "src/delegates/layer_controller_delegate.h"
#include "src/led_controller.h"

namespace threeboard {

// This interface defines the methods used for interacting with a specific layer
// of the threeboard.
class Layer {
public:
  virtual ~Layer() = default;

  Layer(LedController *led_controller) : led_controller_(led_controller) {}

  // Handle a keypress event.
  virtual void HandleEvent(const Keypress &) = 0;

  // Called when the threeboard has transitioned to this layer.
  virtual void TransitionedToLayer() = 0;

  uint8_t Bank0() const { return bank0_; }

  uint8_t Bank1() const { return bank1_; }

protected:
  using LedState = LedController::LedState;

  // TODO: move to cpp
  void UpdateLedState(LayerId layer_id) const {
    switch (layer_id) {
    case LayerId::DFLT:
      led_controller_->SetR(LedState::OFF);
      led_controller_->SetG(LedState::OFF);
      led_controller_->SetB(LedState::OFF);
      break;
    case LayerId::R:
      led_controller_->SetR(LedState::ON);
      led_controller_->SetG(LedState::OFF);
      led_controller_->SetB(LedState::OFF);
      break;
    case LayerId::G:
      led_controller_->SetR(LedState::OFF);
      led_controller_->SetG(LedState::ON);
      led_controller_->SetB(LedState::OFF);
      break;
    case LayerId::B:
      led_controller_->SetR(LedState::OFF);
      led_controller_->SetG(LedState::OFF);
      led_controller_->SetB(LedState::ON);
      break;
    }
    led_controller_->SetBank0(bank0_);
    led_controller_->SetBank1(bank1_);
  }

  LedController *led_controller_;
  uint8_t bank0_ = 0;
  uint8_t bank1_ = 0;
};
} // namespace threeboard