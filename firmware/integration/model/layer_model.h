#include "simulator/simulator_state.h"
#include "src/keypress.h"
#include "src/led_state.h"

namespace threeboard {
namespace integration {
class LayerModel {
 public:
  virtual ~LayerModel() {}
  LayerModel(std::string* usb_buffer) : usb_buffer_(usb_buffer) {}
  virtual bool Apply(const Keypress& keypress) = 0;
  virtual simulator::DeviceState GetStateSnapshot() = 0;

 protected:
  std::string* usb_buffer_;
  simulator::DeviceState device_state_;
};

class DefaultLayerModel : public LayerModel {
 public:
  DefaultLayerModel(std::string* usb_buffer) : LayerModel(usb_buffer) {}

  bool Apply(const Keypress& keypress) override;
  simulator::DeviceState GetStateSnapshot() override;
};
}  // namespace integration
}  // namespace threeboard