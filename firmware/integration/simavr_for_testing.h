#pragma once

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace simulator {

class SimavrForTesting final : public SimavrImpl {
 public:
  static std::unique_ptr<SimavrForTesting> Create(
      absl::flat_hash_map<std::string, uint32_t>* symbol_table);

  absl::Status RunWithTimeout(const std::chrono::milliseconds& timeout);
  absl::Status RunUntilSymbol(const std::string& symbol,
                              const std::chrono::milliseconds& timeout);

  std::vector<uint32_t> GetPrevProgramCounters() const;
  std::vector<uint16_t> GetPrevStackPointers() const;
  void PrintCoreDump() const;

 private:
  SimavrForTesting(absl::flat_hash_map<std::string, uint32_t>* symbol_table,
                   std::unique_ptr<avr_t> avr, uint16_t bss_size,
                   uint16_t data_size);

  void Run() override;
  absl::Status VerifyState() const;

  std::vector<uint32_t> prev_pcs_;
  std::vector<uint16_t> prev_sps_;
  absl::flat_hash_map<std::string, uint32_t>* symbol_table_;
};

}  // namespace simulator
}  // namespace threeboard