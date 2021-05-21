#pragma once

#include <chrono>

#include "gtest/gtest.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace integration {

class SimulatedTestBase : public ::testing::Test {
 public:
  SimulatedTestBase() { simavr_ = simulator::SimavrImpl::Create(); }

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
