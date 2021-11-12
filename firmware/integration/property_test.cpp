#include "gtest/gtest.h"
#include "integration/model/rapidcheck_specializations.h"
#include "integration/model/threeboard_model.h"
#include "integration/util/fake_state_storage.h"
#include "integration/util/testable_simavr.h"
#include "rapidcheck/gtest.h"
#include "simulator/simulator.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

// Auto-generated property-based testing of the threeboard firmware using
// RapidCheck (https://github.com/emil-e/rapidcheck).
class PropertyTest : public testing::Test {
 public:
  PropertyTest() {
    simavr_ =
        TestableSimavr::Create(fake_state_storage_.GetInternalEepromData());
    simulator_ = std::make_unique<simulator::Simulator>(simavr_.get(),
                                                        &fake_state_storage_);
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
      std::cout << " (strlen=" << device_state.usb_buffer.length()
                << ")\nModel:  ";
      rc::show(model_state, std::cout);
      std::cout << " (strlen=" << model_state.usb_buffer.length() << ")"
                << std::endl;
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
    if (keypress == Keypress::Z) {
      simulator_->WaitForUsbOutput(std::chrono::seconds(3));
    }
  }

 protected:
  ThreeboardModel model_;
  FakeStateStorage fake_state_storage_;
  std::unique_ptr<TestableSimavr> simavr_;
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