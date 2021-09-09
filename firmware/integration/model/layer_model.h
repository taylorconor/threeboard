#include "simulator/simulator_state.h"
#include "src/keypress.h"
#include "src/led_state.h"

namespace threeboard {
namespace integration {
class LayerModel {
 public:
  virtual ~LayerModel() {}

  virtual bool Apply(const Keypress& keypress) = 0;
  virtual simulator::DeviceState GetStateSnapshot() = 0;

 protected:
  simulator::DeviceState device_state_;
};

class DefaultLayerModel : public LayerModel {
 public:
  bool Apply(const Keypress& keypress) override;
  simulator::DeviceState GetStateSnapshot() override;
};
}  // namespace integration
}  // namespace threeboard