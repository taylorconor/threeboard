#include <cxxabi.h>

#include <chrono>
#include <unordered_map>

#include "absl/status/status.h"
#include "gtest/gtest.h"
#include "integration/util/fake_state_storage.h"
#include "integration/util/instrumenting_simavr.h"
#include "simulator/simulator.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

class IntegrationTest : public testing::Test {
 public:
  IntegrationTest() {
    internal_eeprom_data_.fill(0xFF);
    simavr_ = simulator::InstrumentingSimavr::Create(&internal_eeprom_data_);
    simulator_ = std::make_unique<simulator::Simulator>(simavr_.get(),
                                                        &fake_state_storage_);
  }

  void ApplyKeypress(const Keypress &keypress) {
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
  std::array<uint8_t, 1024> internal_eeprom_data_{};
  std::unique_ptr<simulator::InstrumentingSimavr> simavr_;
  simulator::FakeStateStorage fake_state_storage_;
  std::unique_ptr<simulator::Simulator> simulator_;
};

TEST_F(IntegrationTest, BootToEventLoop) {
  // Run until the threeboard has successfully started up and is running the
  // event loop. There are potentially millions of cycles here, so we need to
  // set a generous timeout as InstrumentingSimavr is very slow.
  ASSERT_OK(
      simavr_->RunUntilSymbol("threeboard::Threeboard::RunEventLoopIteration",
                              std::chrono::milliseconds(3000)));
}

TEST_F(IntegrationTest, TimerInterruptsFireAfterBooting) {
  // Verify that after beginning event loop iteration, the timer interrupts
  // continue to fire.
  ASSERT_OK(
      simavr_->RunUntilSymbol("threeboard::Threeboard::RunEventLoopIteration",
                              std::chrono::milliseconds(3000)));
  for (int i = 0; i < 10; ++i) {
    ASSERT_OK(
        simavr_->RunUntilSymbol("threeboard::Threeboard::HandleTimer1Interrupt",
                                std::chrono::milliseconds(3000)));
    ASSERT_OK(
        simavr_->RunUntilSymbol("threeboard::Threeboard::HandleTimer3Interrupt",
                                std::chrono::milliseconds(3000)));
  }
}

TEST_F(IntegrationTest, LayerSwitch) {
  ApplyKeypress(Keypress::XYZ);
  auto device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.led_r, true);
  ASSERT_EQ(device_state.led_g, false);
  ASSERT_EQ(device_state.led_b, false);
  ApplyKeypress(Keypress::XYZ);
  device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.led_r, false);
  ASSERT_EQ(device_state.led_g, true);
  ASSERT_EQ(device_state.led_b, false);
  ApplyKeypress(Keypress::XYZ);
  device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.led_r, false);
  ASSERT_EQ(device_state.led_g, false);
  ASSERT_EQ(device_state.led_b, true);
  ApplyKeypress(Keypress::XYZ);
  device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.led_r, false);
  ASSERT_EQ(device_state.led_g, false);
  ASSERT_EQ(device_state.led_b, false);
}

TEST_F(IntegrationTest, DefaultLayerUsbOutput) {
  // Set B0 = 4, B1 = 2.
  std::vector<Keypress> keypresses = {Keypress::X, Keypress::X, Keypress::X,
                                      Keypress::X, Keypress::Y, Keypress::Y};
  for (const Keypress &keypress : keypresses) {
    ApplyKeypress(keypress);
  }

  // Flush to USB and verify that the correct character is output.
  ApplyKeypress(Keypress::Z);
  ASSERT_OK(simavr_->RunUntilNextEventLoopIteration());
  auto device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.usb_buffer, "A");
}

TEST_F(IntegrationTest, LayerRUsbOutput) {
  // Set Layer = R, PROG, shortcut 0 = 4, DFLT.
  std::vector<Keypress> keypresses = {Keypress::XYZ, Keypress::XY, Keypress::X,
                                      Keypress::X,   Keypress::X,  Keypress::X,
                                      Keypress::XYZ};
  for (const Keypress &keypress : keypresses) {
    ApplyKeypress(keypress);
  }

  // Flush and verify.
  ApplyKeypress(Keypress::Z);
  auto device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.led_r, true);
  ASSERT_EQ(device_state.usb_buffer, "a");
}

TEST_F(IntegrationTest, LayerGUsbOutput) {
  // Set Layer = G, PROG, shortcut 0 = {4,5,6}, DFLT.
  std::vector<Keypress> keypresses = {
      Keypress::XYZ, Keypress::XYZ, Keypress::XY, Keypress::X, Keypress::X,
      Keypress::X,   Keypress::X,   Keypress::Z,  Keypress::X, Keypress::Z,
      Keypress::X,   Keypress::Z,   Keypress::XYZ};
  for (const Keypress &keypress : keypresses) {
    ApplyKeypress(keypress);
  }

  ApplyKeypress(Keypress::Z);
  auto device_state = simulator_->GetDeviceState();
  ASSERT_EQ(device_state.led_g, true);
  ASSERT_EQ(device_state.usb_buffer, "abc");
}

TEST_F(IntegrationTest, LayerBUsbOutput) {
  // TODO: Implement once Layer B is finished.
}
}  // namespace
}  // namespace integration
}  // namespace threeboard