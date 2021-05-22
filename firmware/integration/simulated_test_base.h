#pragma once

#include <chrono>

#include "gtest/gtest.h"
#include "simavr/sim_elf.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace integration {

class SimulatedTestBase : public ::testing::Test {
 public:
  SimulatedTestBase() {
    // TODO: without this call to elf_read_firmware, the macOS dynamic linker
    // fails with "dyld: Symbol not found: _elf_read_firmware". This symbol is
    // located within libsimavr.a, but there must be a better way to force this
    // to be linked.
    elf_read_firmware(nullptr, nullptr);
    simavr_ = simulator::SimavrImpl::Create();
  }

  void Run(const std::chrono::milliseconds& timeout =
               std::chrono::milliseconds(10)) {
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      simavr_->Run();
    }
  }

  bool RunUntil(uint8_t data_idx, uint8_t data_val,
                const std::chrono::milliseconds& timeout =
                    std::chrono::milliseconds(10)) {
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      if (simavr_->GetData(data_idx) == data_val) {
        return true;
      }
      simavr_->Run();
    }
    return false;
  }

 protected:
  std::unique_ptr<simulator::Simavr> simavr_;
};

}  // namespace integration
}  // namespace threeboard
