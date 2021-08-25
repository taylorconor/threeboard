#include "integration/model/observable_state.h"
#include "src/keypress.h"
#include "src/led_state.h"

namespace threeboard {
namespace integration {
class LayerModel {
 public:
  virtual ~LayerModel() {}
  LayerModel(std::string* usb_buffer) : usb_buffer_(usb_buffer) {}
  virtual bool Apply(const Keypress& keypress) = 0;
  virtual ObservableState GetStateSnapshot() = 0;

  uint8_t bank0_;
  uint8_t bank1_;
  LedState::State prog_;
  LedState::State status_;
  LedState::State err_;

 protected:
  std::string* usb_buffer_;
};

class DefaultLayerModel : public LayerModel {
 public:
  DefaultLayerModel(std::string* usb_buffer) : LayerModel(usb_buffer) {}

  bool Apply(const Keypress& keypress) override;
  ObservableState GetStateSnapshot() override;
};
}  // namespace integration
}  // namespace threeboard