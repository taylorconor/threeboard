#include <random>

#include "gtest/gtest.h"
#include "integration/model/threeboard_model.h"
#include "integration/util/fake_state_storage.h"
#include "integration/util/testable_simavr.h"
#include "simulator/simulator.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

// Generate a vector of `size` random keypresses.
std::vector<Keypress> GenerateKeypresses(int size) {
  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()};
  std::uniform_int_distribution<int> dist{1, 7};
  std::vector<Keypress> keypresses(size);
  std::generate(keypresses.begin(), keypresses.end(),
                [&dist, &mersenne_engine] {
                  return static_cast<Keypress>(dist(mersenne_engine));
                });
  return keypresses;
}

// A fixture for model-based testing. It creates a testable threeboard simulator
// and a threeboard model, which is a simple threeboard state machine that is
// used to verify if the simulated firmware is in the correct state. This
// fixture is used to facilitate comparison of the simulator and the model in
// property-based tests.
class PropertyTestFixture {
 public:
  PropertyTestFixture() {
    simavr_ =
        TestableSimavr::Create(fake_state_storage_.GetInternalEepromData());
    simulator_ = std::make_unique<simulator::Simulator>(simavr_.get(),
                                                        &fake_state_storage_);
  }

  // Apply the keypress to both the model and the simulator. Returns true if the
  // DeviceState of the model and simulator are equal after the keypress has
  // been applied, and false otherwise.
  bool ApplyAndCompare(const Keypress &keypress) {
    model_.Apply(keypress);
    ApplyToSimulator(keypress);
    auto model_state = model_.GetStateSnapshot();
    auto device_state = simulator_->GetDeviceState();
    bool is_same = (model_state == device_state);
    if (!is_same) {
      std::cout << "Device: " << device_state
                << " (strlen=" << device_state.usb_buffer.length() << ")"
                << std::endl;
      std::cout << "Model:  " << model_state
                << " (strlen=" << model_state.usb_buffer.length() << ")"
                << std::endl;
    }
    return is_same;
  }

 private:
  ThreeboardModel model_;
  FakeStateStorage fake_state_storage_;
  std::unique_ptr<TestableSimavr> simavr_;
  std::unique_ptr<simulator::Simulator> simulator_;

  // Reliably apply the keypress to the simulator, ensuring that enough cycles
  // pass between keydown and keyup so that the keypress registers with the
  // firmware.
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
};

// This test verifies the property that the DeviceState of the simulator matches
// the DeviceState of the model after each keypress. This test runs 25 tests in
// parallel with different auto-generated sequences of 1,000 keypresses each,
// verifying the property after each keypress.
TEST(PropertyTest, DeviceStatePropertyTest) {
  std::vector<std::thread> threads;
  for (int i = 0; i < 25; ++i) {
    threads.emplace_back([] {
      PropertyTestFixture fixture;
      auto keypresses = GenerateKeypresses(1000);
      for (auto keypress : keypresses) {
        ASSERT_TRUE(fixture.ApplyAndCompare(keypress));
      }
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }
}

}  // namespace
}  // namespace integration
}  // namespace threeboard