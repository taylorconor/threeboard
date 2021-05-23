#pragma once

#include <cxxabi.h>

#include <chrono>
#include <unordered_map>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "gtest/gtest.h"
#include "simavr/sim_elf.h"
#include "simulator/components/usb_host.h"
#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator_delegate_mock.h"
#include "util/status_util.h"

namespace threeboard {
namespace integration {

class SimulatedTestBase : public ::testing::Test {
 public:
  SimulatedTestBase() {
    simavr_ = simulator::SimavrImpl::Create();
    usb_host_ = std::make_unique<simulator::UsbHost>(simavr_.get(),
                                                     &simulator_delegate_mock_);
  }

  static void SetUpTestCase() {
    // Parse the symbols from the elf file and build a symbol table out of them.
    BuildSymbolTable();
  }

 protected:
  // TODO: We can get more cycles for a given timeout by optimising the each
  // RunUntil functions loop.
  absl::Status Run(const std::chrono::milliseconds& timeout =
                       std::chrono::milliseconds(100)) {
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      simavr_->Run();
      RETURN_IF_ERROR(VerifyState(simavr_->GetState()));
    }
    return absl::OkStatus();
  }

  absl::Status RunUntil(uint8_t data_idx, uint8_t data_val,
                        const std::chrono::milliseconds& timeout =
                            std::chrono::milliseconds(100)) {
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      if (simavr_->GetData(data_idx) == data_val) {
        return absl::OkStatus();
      }
      simavr_->Run();
      RETURN_IF_ERROR(VerifyState(simavr_->GetState()));
    }
    return absl::DeadlineExceededError(
        absl::StrCat("RunUntil timed out after ", timeout.count(), "ms"));
  }

  absl::Status RunUntil(const std::string& symbol,
                        const std::chrono::milliseconds& timeout =
                            std::chrono::milliseconds(100)) {
    if (!symbol_table_.contains(symbol)) {
      return absl::InternalError(
          absl::StrCat("Symbol '", symbol, "' not found in symbol table"));
    }
    uint32_t stop_addr = symbol_table_[symbol];
    auto start = std::chrono::system_clock::now();
    while (timeout > std::chrono::system_clock::now() - start) {
      if (simavr_->GetProgramCounter() == stop_addr) {
        return absl::OkStatus();
      }
      simavr_->Run();
      RETURN_IF_ERROR(VerifyState(simavr_->GetState()));
    }
    return absl::DeadlineExceededError(
        absl::StrCat("RunUntil timed out after ", timeout.count(), "ms"));
  }

  std::unique_ptr<simulator::Simavr> simavr_;
  std::unique_ptr<simulator::UsbHost> usb_host_;
  simulator::SimulatorDelegateMock simulator_delegate_mock_;

 private:
  static void BuildSymbolTable() {
    elf_firmware_t f;
    if (elf_read_firmware("simulator/native/threeboard_sim_binary.elf", &f)) {
      std::cout << "Failed to parse threeboard ELF file for simulated testing"
                << std::endl;
      exit(0);
    }
    for (int i = 0; i < f.symbolcount; ++i) {
      // Demangle the symbol name.
      avr_symbol_t* symbol = *(f.symbol + i);
      int status;
      char* result = abi::__cxa_demangle(symbol->symbol, 0, 0, &status);
      if (status != 0) {
        // Skip demangling failures.
        continue;
      }

      // The result from __cxa_demangle will leak if not manually deallocated.
      // Convert it to a std::string here and then free it.
      std::string demangled(result);
      free(result);

      // Ignore non-standard symbol names (i.e. those that don't start with
      // `threeboard::`), such as vtable definitions or non-virtual thunks.
      if (!absl::StartsWith(demangled, "threeboard::")) {
        continue;
      }

      // Ignore symbols nested within anonymous namespaces.
      if (absl::StrContains(demangled, "(anonymous namespace")) {
        continue;
      }

      // Ignore the parameter list portion of the symbol signature, and
      // everything that comes after it (CV qualifiers, for example). There's no
      // need to disambiguate between these for now.
      demangled = demangled.substr(0, demangled.find('('));
      symbol_table_[demangled] = symbol->addr;
    }
  }

  absl::Status VerifyState(uint8_t state) {
    if (state == simulator::CRASHED || state == simulator::DONE) {
      return absl::InternalError(absl::StrCat(
          "Simulator entered error state: ", state, ". Final instruction: 0x",
          absl::Hex(simavr_->GetPrevProgramCounter()), "."));
    }
    if (simavr_->GetProgramCounter() == 0 &&
        simavr_->GetPrevProgramCounter() != 0) {
      return absl::InternalError(absl::StrCat(
          "Simulator unexpectedly jumped to 0x0. Final instruction: 0x",
          absl::Hex(simavr_->GetPrevProgramCounter()), "."));
    }
    return absl::OkStatus();
  }

  static absl::flat_hash_map<std::string, uint32_t> symbol_table_;
};

absl::flat_hash_map<std::string, uint32_t> SimulatedTestBase::symbol_table_;

}  // namespace integration
}  // namespace threeboard
