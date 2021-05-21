#pragma once

#include "gtest/gtest.h"
#include "simulator/simavr/simavr_impl.h"

namespace threeboard {
namespace simulator {

class SimulatedTestBase : public ::testing::Test {
 public:
  SimulatedTestBase() { simavr_ = SimavrImpl::Create(); }

  void Run(const std::chrono::milliseconds& timeout =
               std::chrono::milliseconds(10)) {
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      simavr_->Run();
    }
  }

  void RunUntil(uint8_t data_idx, uint8_t data_val,
                const std::chrono::milliseconds& timeout =
                    std::chrono::milliseconds(10)) {
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      if (simavr_->GetData(data_idx) == data_val) {
        return;
      }
      simavr_->Run();
    }
  }

 protected:
  std::unique_ptr<Simavr> simavr_;
};

}  // namespace simulator
}  // namespace threeboard