#include <array>
#include <vector>

#include "simulator/simulator_state.h"
#include "src/keypress.h"
#include "src/led_state.h"

namespace threeboard {
namespace integration {

// An interface defining the properties all layers should provide in their
// models. All a layer model implementation needs to do is apply keypresses to
// itself and provide a way to get a snapshot of the current state.
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
  std::array<std::vector<char>, 256> shortcuts_;

  std::string ApplyModCodeToCurrentShortcut();
};

class LayerBModel : public LayerModel {
 public:
  bool Apply(const Keypress& keypress) override;
  simulator::DeviceState GetStateSnapshot() override;

 private:
  std::string usb_buffer_;
  uint8_t shortcut_id_ = 0;
  uint8_t key_code_ = 0;
  uint8_t mod_code_ = 0;
  bool prog_ = false;
  std::array<std::vector<char>, 248> shortcuts_;
};
}  // namespace integration
}  // namespace threeboard