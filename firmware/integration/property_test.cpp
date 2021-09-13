#include "gtest/gtest.h"
#include "integration/model/rapidcheck_specializations.h"
#include "integration/model/threeboard_model.h"
#include "rapidcheck/gtest.h"
#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"

namespace threeboard {
namespace integration {
namespace {

std::vector<char> GetKeycodes(const Keypress &keypress) {
  switch (keypress) {
    case Keypress::X:
      return {'a'};
    case Keypress::Y:
      return {'s'};
    case Keypress::Z:
      return {'d'};
    case Keypress::XY:
      return {'a', 's'};
    case Keypress::XZ:
      return {'a', 'd'};
    case Keypress::YZ:
      return {'s', 'd'};
    case Keypress::XYZ:
      return {'a', 's', 'd'};
    default:
      return {};
  }
}

class PropertyTest : public testing::Test {
 public:
  PropertyTest() {
    internal_eeprom_data_.fill(0xFF);
    simavr_ = simulator::SimavrImpl::Create(&internal_eeprom_data_);
    simulator_ = std::make_unique<simulator::Simulator>(simavr_.get(), nullptr);
    simulator_->RunAsync();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  bool ApplyAndCompare(const Keypress &keypress) {
    model_.Apply(keypress);
    ApplyToSimulator(keypress);
    auto model_state = model_.GetStateSnapshot();
    auto device_state = simulator_->GetDeviceState();
    return model_state == device_state;
  }

 protected:
  ThreeboardModel model_;
  std::array<uint8_t, 1024> internal_eeprom_data_;
  std::unique_ptr<simulator::Simavr> simavr_;
  std::unique_ptr<simulator::Simulator> simulator_;

  void ApplyToSimulator(const Keypress &keypress) {
    auto keycodes = GetKeycodes(keypress);
    for (char keycode : keycodes) {
      simulator_->HandleKeypress(keycode, true);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    for (char keycode : keycodes) {
      simulator_->HandleKeypress(keycode, false);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
  }
};

TEST_F(PropertyTest, DefaultLayerUsbOutput) {
  // Set B0 = 4, B1 = 2.
  std::vector<Keypress> keypresses = {Keypress::X, Keypress::X, Keypress::X,
                                      Keypress::X, Keypress::Y, Keypress::Y};
  for (const Keypress &keypress : keypresses) {
    ASSERT_TRUE(ApplyAndCompare(keypress));
  }

  // Flush to USB output, verify consistency with the model, and verify that the
  // correct character was output.
  model_.Apply(Keypress::Z);
  ApplyToSimulator(Keypress::Z);
  auto model_state = model_.GetStateSnapshot();
  auto device_state = simulator_->GetDeviceState();
  ASSERT_EQ(model_state, device_state);
  ASSERT_EQ(device_state.usb_buffer, "A");
}

TEST_F(PropertyTest, LayerRUsbOutput) {
  // Set Layer = R, PROG, shortcut 0 = 4, DFLT.
  std::vector<Keypress> keypresses = {Keypress::XYZ, Keypress::XY, Keypress::X,
                                      Keypress::X,   Keypress::X,  Keypress::X,
                                      Keypress::XYZ};
  for (const Keypress &keypress : keypresses) {
    ASSERT_TRUE(ApplyAndCompare(keypress));
  }

  // Flush shortcut to USB and verify the correct value was output.
  model_.Apply(Keypress::Z);
  ApplyToSimulator(Keypress::Z);
  auto model_state = model_.GetStateSnapshot();
  auto device_state = simulator_->GetDeviceState();
  ASSERT_EQ(model_state, device_state);
  ASSERT_EQ(device_state.usb_buffer, "a");
}

RC_GTEST_FIXTURE_PROP(PropertyTest, DefaultPropertyTest,
                      (const std::vector<Keypress> &keypresses)) {
  for (const Keypress &keypress : keypresses) {
    RC_ASSERT(ApplyAndCompare(keypress));
  }
}

}  // namespace
}  // namespace integration
}  // namespace threeboard