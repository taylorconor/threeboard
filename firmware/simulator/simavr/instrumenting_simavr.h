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

  ~InstrumentingSimavr() override {
    std::cout << "Integrity cycles: " << integrity_cycles_
              << ", no integrity cycles: " << no_integrity_cycles_ << std::endl;
  }

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
  std::vector<uint8_t> CopyDataSegment() const;
  bool ShouldRunIntegrityCheckAtCurrentCycle() const;
  absl::Status RunWithIntegrityChecks();

  bool finished_do_copy_data_ = false;

  uint16_t data_start_;
  std::vector<uint32_t> prev_pcs_;
  std::vector<uint16_t> prev_sps_;

  mutable int integrity_cycles_ = 0;
  mutable int no_integrity_cycles_ = 0;

  absl::flat_hash_map<std::string, SymbolInfo>* symbol_table_;
};

}  // namespace simulator
}  // namespace threeboard