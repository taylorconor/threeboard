#include "simulator/simulator_state.h"
#include "src/keypress.h"
#include "src/led_state.h"

namespace threeboard {
namespace integration {
class LayerModel {
 public:
  virtual ~LayerModel() {}

  virtual bool Apply(const Keypress& keypress) = 0;
  virtual simulator::DeviceState GetStateSnapshot();

 protected:
  simulator::DeviceState device_state_;
};

class DefaultLayerModel : public LayerModel {
 public:
  bool Apply(const Keypress& keypress) override;
};

class LayerRModel : public LayerModel {
 public:
  bool Apply(const Keypress& keypress) override;
  simulator::DeviceState GetStateSnapshot() override;

 private:
  std::string usb_buffer_;
  uint8_t shortcut_id_ = 0;
  uint8_t modcode_ = 0;
  bool prog_ = false;
  uint8_t shortcuts_[256] = {0};
};

class LayerGModel : public LayerModel {
 public:
  bool Apply(const Keypress& keypress) override;
  simulator::DeviceState GetStateSnapshot() override;

 private:
  std::string usb_buffer_;
  uint8_t shortcut_id_ = 0;
  uint8_t word_mod_code_ = 0;
  uint8_t key_code_ = 0;
  bool prog_ = false;
  std::string shortcuts_[256];
  uint8_t shortcut_lengths_[256] = {0};
};

class LayerBModel : public DefaultLayerModel {
 public:
  simulator::DeviceState GetStateSnapshot() override {
    auto state = DefaultLayerModel::GetStateSnapshot();
    state.led_b = true;
    return state;
  }
};
}  // namespace integration
}  // namespace threeboard