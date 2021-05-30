#pragma once

#include <vector>

#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace simulator {

class SimavrForTesting final : public SimavrImpl {
 public:
  static std::unique_ptr<SimavrForTesting> Create();

  void Run() override;

  std::vector<uint32_t> GetPrevProgramCounters() const;
  std::vector<uint16_t> GetPrevStackPointers() const;
  void PrintCoreDump() const;

 private:
  SimavrForTesting(std::unique_ptr<avr_t> avr, uint16_t bss_size,
                   uint16_t data_size);

  std::vector<uint32_t> prev_pcs_;
  std::vector<uint16_t> prev_sps_;
};

}  // namespace simulator
}  // namespace threeboard