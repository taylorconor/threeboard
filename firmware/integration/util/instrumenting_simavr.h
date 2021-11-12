#pragma once

#include <vector>

#include "testable_simavr.h"

namespace threeboard {
namespace integration {

// An extension of TestableSimavr that adds runtime instrumentation to the
// simulated firmware. This class is used for simulator-backed integration
// tests, using its instrumentation to verify the firmware for integrity. It is
// considerably slower than TestableSimavr due to the heavy overhead of runtime
// integrity checks.
class InstrumentingSimavr final : public TestableSimavr {
 public:
  ~InstrumentingSimavr() override = default;

  static std::unique_ptr<InstrumentingSimavr> Create(
      std::array<uint8_t, 1024>* internal_eeprom_data);

  absl::Status RunWithChecks() override;

 private:
  InstrumentingSimavr(std::unique_ptr<avr_t> avr,
                      std::unique_ptr<elf_firmware_t> elf_firmware);

  void PrintCoreDump() const;
  void CopyDataSegment(std::vector<uint8_t>*) const;
  bool ShouldRunIntegrityCheckAtCurrentCycle() const;

  bool finished_do_copy_data_ = false;

  uint16_t data_start_;
  std::vector<uint32_t> prev_pcs_;
  std::vector<uint16_t> prev_sps_;

  // This vector is used within RunWithChecks, and is repeatedly overwritten
  // hundreds of thousands of times. For performance, we preserve it here for
  // the lifetime of the instance, so we don't have to continuously free and
  // reallocate its memory.
  mutable std::vector<uint8_t> data_before_;
};

}  // namespace integration
}  // namespace threeboard