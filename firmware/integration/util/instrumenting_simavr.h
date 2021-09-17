#pragma once

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace simulator {

// An extension of SimavrImpl that adds optional runtime instrumentation to the
// simulated firmware. This class is used for simulator-backed integration
// tests, using its instrumentation to verify the firmware for integrity. This
// class should not be used for interactive simulation while instrumenting, as
// its runtime instrumentation makes it considerably slower than SimavrImpl.
class InstrumentingSimavr final : public SimavrImpl {
 public:
  ~InstrumentingSimavr() override = default;

  static std::unique_ptr<InstrumentingSimavr> Create(
      std::unique_ptr<elf_firmware_t> firmware,
      std::array<uint8_t, 1024>* internal_eeprom_data);

  absl::Status RunWithTimeout(const std::chrono::milliseconds& timeout);
  absl::Status RunUntilSymbol(const std::string& symbol,
                              const std::chrono::milliseconds& timeout);

  std::vector<uint32_t> GetPrevProgramCounters() const;
  std::vector<uint16_t> GetPrevStackPointers() const;
  void PrintCoreDump() const;

 private:
  InstrumentingSimavr(std::unique_ptr<avr_t> avr,
                      std::unique_ptr<elf_firmware_t> elf_firmware);

  void Run() override;
  void CopyDataSegment(std::vector<uint8_t>*) const;
  bool ShouldRunIntegrityCheckAtCurrentCycle() const;
  absl::Status RunWithIntegrityChecks();
  static void BuildSymbolTable(elf_firmware_t* firmware);

  bool finished_do_copy_data_ = false;

  uint16_t data_start_;
  std::vector<uint32_t> prev_pcs_;
  std::vector<uint16_t> prev_sps_;

  // This vector is used within RunWithIntegrityChecks, and is repeatedly
  // overwritten hundreds of thousands of times. For performance we preserve it
  // here for the lifetime of the instance, so we don't have to continuously
  // free and reallocate its memory.
  mutable std::vector<uint8_t> data_before_;

  static absl::flat_hash_map<std::string, avr_symbol_t*> symbol_table_;
};

}  // namespace simulator
}  // namespace threeboard