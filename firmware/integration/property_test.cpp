#include "gtest/gtest.h"
#include "integration/model/rapidcheck_specializations.h"
#include "integration/model/threeboard_model.h"
#include "integration/util/instrumenting_simavr.h"
#include "rapidcheck/gtest.h"
#include "simulator/simulator.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

class PropertyTest : public testing::Test {
 public:
  PropertyTest() {
    internal_eeprom_data_.fill(0xFF);
    simavr_ = simulator::InstrumentingSimavr::Create(&internal_eeprom_data_);
    simulator_ = std::make_unique<simulator::Simulator>(simavr_.get(), nullptr);
  }

  bool ApplyAndCompare(const Keypress &keypress) {
    model_.Apply(keypress);
    ApplyToSimulator(keypress);
    auto model_state = model_.GetStateSnapshot();
    auto device_state = simulator_->GetDeviceState();
    bool is_same = (model_state == device_state);
    if (!is_same) {
      std::cout << "Device: ";
      rc::show(device_state, std::cout);
      std::cout << "\nModel:  ";
      rc::show(model_state, std::cout);
      std::cout << std::endl;
    }
    return is_same;
  }

  void ApplyToSimulator(const Keypress &keypress) {
    EXPECT_OK(simavr_->RunUntilStartKeypressProcessing());
    simulator_->HandleKeypress(keypress, true);
    EXPECT_OK(simavr_->RunUntilNextEventLoopIteration());
    EXPECT_OK(simavr_->RunUntilStartKeypressProcessing());
    simulator_->HandleKeypress(keypress, false);
    // Run until we can guarantee that all aspects of the device state have had
    // the opportunity to update.
    EXPECT_OK(simavr_->RunUntilFullLedRefresh());
    EXPECT_OK(simavr_->RunUntilNextEventLoopIteration());
  }

 protected:
  ThreeboardModel model_;
  std::array<uint8_t, 1024> internal_eeprom_data_;
  std::unique_ptr<simulator::InstrumentingSimavr> simavr_;
  std::unique_ptr<simulator::Simulator> simulator_;
};

RC_GTEST_FIXTURE_PROP(PropertyTest, DefaultPropertyTest,
                      (const std::vector<Keypress> &keypresses)) {
  for (const Keypress &keypress : keypresses) {
    RC_ASSERT(ApplyAndCompare(keypress));
  }
}

}  // namespace
}  // namespace integration
}  // namespace threeboard