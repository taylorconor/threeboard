#include "led_controller.h"

#include <memory>

#include "src/native/native_mock.h"

namespace threeboard {
namespace {

class LedControllerTest : public ::testing::Test {
 public:
  LedControllerTest() {
    EXPECT_CALL(native_mock_, EnableDDRB(0b01110000)).Times(1);
    EXPECT_CALL(native_mock_, EnableDDRC(0b01000000)).Times(1);
    EXPECT_CALL(native_mock_, EnableDDRD(0b11010000)).Times(1);
    EXPECT_CALL(native_mock_, EnableDDRF(0b00110011)).Times(1);
    controller_ = std::make_unique<LedController>(&native_mock_);
  }

  void MockDefaultScanLine(int times) {
    EXPECT_CALL(native_mock_, DisablePORTB(0b00110000)).Times(times);
    EXPECT_CALL(native_mock_, DisablePORTD(0b11010000)).Times(times);
    EXPECT_CALL(native_mock_, EnablePORTF(0b00110011)).Times(times);
  }

  void MockDefaultStatusLEDs(int full_refreshes) {
    EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB6))
        .Times(full_refreshes);
    EXPECT_CALL(native_mock_, DisablePORTC(1 << native::PC6))
        .Times(full_refreshes * 3);
  }

  native::NativeMock native_mock_;
  std::unique_ptr<LedController> controller_;
};

TEST_F(LedControllerTest, TestStatusAndErr) {
  MockDefaultScanLine(2);
  // PD7 is the row enabled for row 0.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB6)).Times(1);
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
  controller_->GetLedState()->SetErr(LedState::ON);
  controller_->ScanNextLine();

  // PB4 is the row enabled for row 1.
  EXPECT_CALL(native_mock_, EnablePORTC(1 << native::PC6)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB6)).Times(1);
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB4)).Times(1);
  controller_->GetLedState()->SetErr(LedState::OFF);
  controller_->GetLedState()->SetStatus(LedState::ON);
  controller_->ScanNextLine();
}

TEST_F(LedControllerTest, TestCorrectRowPinsEnabled) {
  MockDefaultScanLine(6);
  MockDefaultStatusLEDs(1);
  // Row 0.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
  controller_->ScanNextLine();
  // Row 1.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB4)).Times(1);
  controller_->ScanNextLine();
  // Row 2.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD6)).Times(1);
  controller_->ScanNextLine();
  // Row 3.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD4)).Times(1);
  controller_->ScanNextLine();
  // Row 4.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB5)).Times(1);
  controller_->ScanNextLine();
  // Wraparound to row 0.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
  controller_->ScanNextLine();
}

TEST_F(LedControllerTest, TestCorrectColumnPinsDisabled) {
  MockDefaultScanLine(6);
  MockDefaultStatusLEDs(1);
  controller_->GetLedState()->SetBank0(0b00100001);
  controller_->GetLedState()->SetBank1(0b10000100);
  controller_->GetLedState()->SetR(LedState::ON);
  // Row 0.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF4)).Times(1);
  controller_->ScanNextLine();
  // Row 1.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB4)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF5)).Times(1);
  controller_->ScanNextLine();
  // Row 2.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD6)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF0)).Times(1);
  controller_->ScanNextLine();
  // Row 3.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD4)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF1)).Times(1);
  controller_->ScanNextLine();
  // Row 4.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB5)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF0)).Times(1);
  controller_->ScanNextLine();
  // Wraparound to row 0.
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF4)).Times(1);
  controller_->ScanNextLine();
}
}  // namespace
}  // namespace threeboard
