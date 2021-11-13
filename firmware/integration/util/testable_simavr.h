#pragma once

#include <mutex>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace integration {

// An extension of SimavrImpl that adds a layer of useful utilities for
// integration and property-based test suites. It uses a threeboard firmware
// binary compiled with a much higher CPU frequency (F_CPU), to make the
// firmware run faster than realtime inside Simavr. TestableSimavr should only
// be used in tests as it has a non-trivial runtime overhead.
class TestableSimavr : public simulator::SimavrImpl {
 public:
  ~TestableSimavr() override = default;

  static std::unique_ptr<TestableSimavr> Create(
      std::array<uint8_t, 1024>* internal_eeprom_data);

  virtual absl::Status RunWithChecks();

  absl::Status RunUntilStartKeypressProcessing();
  absl::Status RunUntilFullLedRefresh();
  absl::Status RunUntilNextEventLoopIteration();
  absl::Status RunUntilSymbol(const std::string& symbol,
                              const std::chrono::milliseconds& timeout);

 protected:
  TestableSimavr(std::unique_ptr<avr_t> avr,
                 std::unique_ptr<elf_firmware_t> elf_firmware);

  static void BuildSymbolTable(elf_firmware_t* firmware);

  static std::mutex symbol_table_mutex_;
  static absl::flat_hash_map<std::string, avr_symbol_t*> symbol_table_;
};

}  // namespace integration
}  // namespace threeboard