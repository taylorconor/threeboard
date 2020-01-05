#include "led_controller.h"
#include "native/native_mock.h"
#include <memory>

namespace threeboard {
namespace util {
namespace {

using LedState = LedController::LedState;

class LedControllerTest : public ::testing::Test {
public:
  LedControllerTest() {
    EXPECT_CALL(native_mock_, EnableDDRB(0b00110000)).Times(1);
    EXPECT_CALL(native_mock_, EnableDDRD(0b11010000)).Times(1);
    EXPECT_CALL(native_mock_, EnableDDRF(0b11110011)).Times(1);
    controller_ = std::make_unique<LedController>(&native_mock_);
  }

  void MockDefaultScanLine(int times) {
    EXPECT_CALL(native_mock_, DisablePORTB(0b00100000)).Times(times);
    EXPECT_CALL(native_mock_, EnablePORTB(0b00010000)).Times(times);
    EXPECT_CALL(native_mock_, DisablePORTF(0b00110011)).Times(times);
    EXPECT_CALL(native_mock_, EnablePORTD(0b11010000)).Times(times);
  }

  void MockDefaultStatusLEDs(int full_refreshes) {
    EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF6))
        .Times(full_refreshes);
    EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF7))
        .Times(full_refreshes * 3);
  }

  native::NativeMock native_mock_;
  std::unique_ptr<LedController> controller_;
};

TEST_F(LedControllerTest, TestStatusAndErr) {
  MockDefaultScanLine(2);
  // PF5 is the row enabled for row 0.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF5)).Times(1);
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF6)).Times(1);
  controller_->SetErr(LedState::ON);
  controller_->ScanNextLine();

  // PF4 is the row enabled for row 1.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF4)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF6)).Times(1);
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF7)).Times(1);
  controller_->SetErr(LedState::OFF);
  controller_->SetStatus(LedState::ON);
  controller_->ScanNextLine();
}

TEST_F(LedControllerTest, TestCorrectRowPinsEnabled) {
  MockDefaultScanLine(6);
  MockDefaultStatusLEDs(1);
  // Row 1.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF5)).Times(1);
  controller_->ScanNextLine();
  // Row 2.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF4)).Times(1);
  controller_->ScanNextLine();
  // Row 3.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF1)).Times(1);
  controller_->ScanNextLine();
  // Row 4.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF0)).Times(1);
  controller_->ScanNextLine();
  // Row 5.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB5)).Times(1);
  controller_->ScanNextLine();
  // Wraparound to row 1.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF5)).Times(1);
  controller_->ScanNextLine();
}

TEST_F(LedControllerTest, TestCorrectColumnPinsDisabled) {
  MockDefaultScanLine(6);
  MockDefaultStatusLEDs(1);
  controller_->SetBank0(0b00100001);
  controller_->SetBank1(0b10000100);
  controller_->SetR(LedState::ON);
  // Row 1.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF5)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTD(1 << native::PD7)).Times(1);
  controller_->ScanNextLine();
  // Row 2.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF4)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB4)).Times(1);
  controller_->ScanNextLine();
  // Row 3.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF1)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTD(1 << native::PD4)).Times(1);
  controller_->ScanNextLine();
  // Row 4.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF0)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTD(1 << native::PD6)).Times(1);
  controller_->ScanNextLine();
  // Row 5.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB5)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTD(1 << native::PD4)).Times(1);
  controller_->ScanNextLine();
  // Wraparound to row 1.
  EXPECT_CALL(native_mock_, EnablePORTF(1 << native::PF5)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTD(1 << native::PD7)).Times(1);
  controller_->ScanNextLine();
}
} // namespace
} // namespace util
} // namespace threeboard
