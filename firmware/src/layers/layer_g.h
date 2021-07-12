#pragma once

#include "src/delegates/layer_controller_delegate.h"
#include "src/layers/layer.h"
#include "src/storage/storage_controller.h"

namespace threeboard {

class LayerG final : public Layer {
 public:
  LayerG(LedState *led_state, usb::UsbController *usb_controller,
         storage::StorageController *storage_controller,
         LayerControllerDelegate *layer_controller_delegate)
      : Layer(led_state, usb_controller),
        layer_controller_delegate_(layer_controller_delegate),
        storage_controller_(storage_controller) {}

  bool HandleEvent(const Keypress &) override;

  // Called when the threeboard has transitioned to this layer.
  void TransitionedToLayer() override;

 private:
  LayerControllerDelegate *layer_controller_delegate_;
  storage::StorageController *storage_controller_;

  uint8_t shortcut_id_ = 0;
  uint8_t word_mod_code_ = 0;
  uint8_t key_code_ = 0;
};

}  // namespace threeboard
