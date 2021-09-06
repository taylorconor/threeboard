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
    std::array<uint8_t, 1024> internal_eeprom_data{};
    simavr_ = simulator::SimavrImpl::Create(&internal_eeprom_data);
    simulator_ = std::make_unique<simulator::Simulator>(simavr_.get(), nullptr);
    simulator_->RunAsync();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    simulator_->GetStateAndFlush();
  }

  void ApplyToSimulator(const Keypress &keypress) {
    auto keycodes = GetKeycodes(keypress);
    for (char keycode : keycodes) {
      simulator_->HandleKeypress(keycode, true);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    for (char keycode : keycodes) {
      simulator_->HandleKeypress(keycode, false);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    simulator_->GetStateAndFlush();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

 protected:
  ThreeboardModel model_;
  std::unique_ptr<simulator::Simavr> simavr_;
  std::unique_ptr<simulator::Simulator> simulator_;
};

RC_GTEST_FIXTURE_PROP(PropertyTest, DefaultPropertyTest,
                      (const std::vector<Keypress> &keypresses)) {
  for (const Keypress &keypress : keypresses) {
    model_.Apply(keypress);
    ApplyToSimulator(keypress);

    auto model_state = model_.GetStateSnapshot();
    auto simulator_state = simulator_->GetStateAndFlush();
    RC_ASSERT(model_state == simulator_state.device_state);
  }
}

}  // namespace
}  // namespace integration
}  // namespace threeboard