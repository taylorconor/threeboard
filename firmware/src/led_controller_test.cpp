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

  void RunAndExpectFullScanWithErrState(bool enabled) {
    MockDefaultScanLine(5);
    // Row 0.
    EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
    controller_->ScanNextLine();
    // Row 1.
    EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB4)).Times(1);
    EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB6)).Times(1);
    EXPECT_CALL(native_mock_, EnablePORTC(1 << native::PC6))
        .Times(enabled ? 1 : 0);
    controller_->ScanNextLine();
    // Row 2.
    EXPECT_CALL(native_mock_, DisablePORTC(1 << native::PC6)).Times(1);
    EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD6)).Times(1);
    controller_->ScanNextLine();
    // Row 3.
    EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD4)).Times(1);
    controller_->ScanNextLine();
    // Row 4.
    EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB5)).Times(1);
    controller_->ScanNextLine();
  }

  void MockDefaultStatusLEDs(int full_refreshes) {
    EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB6))
        .Times(full_refreshes);
    EXPECT_CALL(native_mock_, DisablePORTC(1 << native::PC6))
        .Times(full_refreshes);
  }

  native::NativeMock native_mock_;
  std::unique_ptr<LedController> controller_;
};

TEST_F(LedControllerTest, TestStatusAndErr) {
  MockDefaultScanLine(2);
  // PD7 is the row enabled for row 0.
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB6)).Times(1);
  EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
  controller_->GetLedState()->SetStatus(LedState::ON);
  controller_->ScanNextLine();

  // PB4 is the row enabled for row 1.
  EXPECT_CALL(native_mock_, EnablePORTC(1 << native::PC6)).Times(1);
  EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB6)).Times(1);
  EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB4)).Times(1);
  controller_->GetLedState()->SetStatus(LedState::OFF);
  controller_->GetLedState()->SetErr(LedState::ON);
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

TEST_F(LedControllerTest, TestBlink) {
  // Increment blink status to just below the blink threshold.
  for (int i = 0; i < 0x3F; i++) {
    controller_->UpdateBlinkStatus();
  }

  // Setting to BLINK will initially cause the LED to turn off, since the
  // blink_state starts below the threshold.
  controller_->GetLedState()->SetErr(LedState::BLINK);
  RunAndExpectFullScanWithErrState(false);

  // Increment Blink status to 0x40 (the blink threshold).
  controller_->UpdateBlinkStatus();

  // Now scan line 0 will produce a state of ON for the ERR LED.
  RunAndExpectFullScanWithErrState(true);

  // Increment the blink status another 0x40 so it's out of the blink threshold
  // again.
  for (int i = 0; i < 0x40; i++) {
    controller_->UpdateBlinkStatus();
  }

  // Now scan line 0 will produce a state of OFF again for the ERR LED.
  RunAndExpectFullScanWithErrState(false);
}
}  // namespace
}  // namespace threeboard
