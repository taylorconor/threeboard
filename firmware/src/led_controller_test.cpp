#include "led_controller.h"

#include <memory>
#include <optional>

#include "src/native/native_mock.h"

namespace threeboard {
namespace {

constexpr std::nullopt_t _ = std::nullopt;
using ::testing::AtMost;

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

  void RunFullScanWithExpectations(std::optional<bool> prog,
                                   std::optional<bool> status,
                                   std::optional<bool> err) {
    MockDefaultScanLine(5);
    // Row 0.
    EXPECT_CALL(native_mock_, EnablePORTD(1 << native::PD7)).Times(1);
    if (status.has_value()) {
      EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB6))
          .Times(*status ? 1 : 0);
    } else {
      EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB6)).Times(AtMost(1));
    }
    controller_->ScanNextLine();
    // Row 1.
    EXPECT_CALL(native_mock_, EnablePORTB(1 << native::PB4)).Times(1);
    EXPECT_CALL(native_mock_, DisablePORTB(1 << native::PB6)).Times(1);
    if (err.has_value()) {
      EXPECT_CALL(native_mock_, EnablePORTC(1 << native::PC6))
          .Times(*err ? 1 : 0);
    } else {
      EXPECT_CALL(native_mock_, EnablePORTC(1 << native::PC6)).Times(AtMost(1));
    }
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
    if (prog.has_value()) {
      EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF5))
          .Times(*prog ? 1 : 0);
    } else {
      EXPECT_CALL(native_mock_, DisablePORTF(1 << native::PF5))
          .Times(AtMost(1));
    }
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
  RunFullScanWithExpectations(_, _, false);

  // Increment Blink status to 0x40 (the blink threshold).
  controller_->UpdateBlinkStatus();

  // Now scan line 0 will produce a state of ON for the ERR LED.
  RunFullScanWithExpectations(_, _, true);

  // Increment the blink status another 0x40 so it's out of the blink threshold
  // again.
  for (int i = 0; i < 0x40; i++) {
    controller_->UpdateBlinkStatus();
  }

  // Now scan line 0 will produce a state of OFF again for the ERR LED.
  RunFullScanWithExpectations(_, _, false);
}

TEST_F(LedControllerTest, TestPulse) {
  // Set ERR to PULSE and verify that it is on in the next cycle.
  controller_->GetLedState()->SetErr(LedState::PULSE);
  RunFullScanWithExpectations(false, _, true);

  // Now set PROG to PULSE and verify both are now on.
  controller_->GetLedState()->SetProg(LedState::PULSE);
  RunFullScanWithExpectations(true, _, true);

  // Continue running full scans until the pulse timer for the ERR LED expires.
  // It's 6 bits wide, so it takes 63 cycles. We've already run two of them.
  for (int i = 0; i < 61; ++i) {
    RunFullScanWithExpectations(true, _, true);
  }

  // On this scan, the ERR LED's PULSE state will expire and it will revert to
  // OFF, but PROG will remain.
  RunFullScanWithExpectations(true, _, false);
  ASSERT_EQ(controller_->GetLedState()->GetErr()->state, LedState::OFF);

  // Now PROG reverts to OFF.
  RunFullScanWithExpectations(false, _, false);
  ASSERT_EQ(controller_->GetLedState()->GetProg()->state, LedState::OFF);
}
}  // namespace
}  // namespace threeboard
