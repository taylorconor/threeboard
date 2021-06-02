#pragma once

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace simulator {

class InstrumentingSimavr final : public SimavrImpl {
 public:
  struct SymbolInfo {
    uint32_t address;
    uint32_t size;

    SymbolInfo() : address(0), size(0) {}
    SymbolInfo(uint32_t address) : address(address), size(0) {}
  };

  ~InstrumentingSimavr() override = default;

  static std::unique_ptr<InstrumentingSimavr> Create(
      elf_firmware_t* elf_firmware,
      absl::flat_hash_map<std::string, SymbolInfo>* symbol_table);

  absl::Status RunWithTimeout(const std::chrono::milliseconds& timeout);
  absl::Status RunUntilSymbol(const std::string& symbol,
                              const std::chrono::milliseconds& timeout);

  std::vector<uint32_t> GetPrevProgramCounters() const;
  std::vector<uint16_t> GetPrevStackPointers() const;
  void PrintCoreDump() const;

 private:
  InstrumentingSimavr(
      elf_firmware_t* elf_firmware,
      absl::flat_hash_map<std::string, SymbolInfo>* symbol_table,
      std::unique_ptr<avr_t> avr);

  void Run() override;
  void CopyDataSegment(std::vector<uint8_t>*) const;
  bool ShouldRunIntegrityCheckAtCurrentCycle() const;
  absl::Status RunWithIntegrityChecks();

  bool finished_do_copy_data_ = false;

  uint16_t data_start_;
  std::vector<uint32_t> prev_pcs_;
  std::vector<uint16_t> prev_sps_;

  // This vector is used within RunWithIntegrityChecks, and is repeatedly
  // overwritten hundreds of thousands of times. For performance we preserve it
  // here for the lifetime of the instance, so we don't have to continuously
  // free and reallocate its memory.
  mutable std::vector<uint8_t> data_before_;

  absl::flat_hash_map<std::string, SymbolInfo>* symbol_table_;
};

}  // namespace simulator
}  // namespace threeboard