#pragma once

#include "src/delegates/layer_controller_delegate.h"
#include "src/layers/layer.h"
#include "src/storage/storage_controller.h"

namespace threeboard {

class LayerR final : public Layer {
 public:
  LayerR(LedState *led_state, usb::UsbController *usb_controller,
         storage::StorageController *storage_controller,
         LayerControllerDelegate *layer_controller_delegate)
      : Layer(led_state, usb_controller),
        layer_controller_delegate_(layer_controller_delegate),
        storage_controller_(storage_controller) {}

  bool HandleEvent(const Keypress &) override;

  // Called when the threeboard has transitioned to this layer.
  bool TransitionedToLayer() override;

 private:
  LayerControllerDelegate *layer_controller_delegate_;
  storage::StorageController *storage_controller_;

  uint8_t shortcut_id_ = 0;
  uint8_t modcode_ = 0;
  uint8_t current_prog_char_ = 0;
};

}  // namespace threeboard
