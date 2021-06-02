#include <cxxabi.h>

#include <chrono>
#include <unordered_map>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "gtest/gtest.h"
#include "simavr/sim_elf.h"
#include "simulator/components/usb_host.h"
#include "simulator/simavr/instrumenting_simavr.h"
#include "simulator/simulator_delegate_mock.h"
#include "util/gtest_util.h"
#include "util/status_util.h"

namespace threeboard {
namespace integration {
namespace {

using SymbolInfo = simulator::InstrumentingSimavr::SymbolInfo;

class IntegrationTest : public testing::Test {
 public:
  IntegrationTest()
      : simavr_(simulator::InstrumentingSimavr::Create(&firmware_,
                                                       &symbol_table_)) {
    usb_host_ = std::make_unique<simulator::UsbHost>(simavr_.get(),
                                                     &simulator_delegate_mock_);
  }

  static void SetUpTestCase() {
    if (elf_read_firmware("simulator/native/threeboard_sim_binary.elf",
                          &firmware_)) {
      std::cout << "Failed to parse threeboard ELF file for simulated testing"
                << std::endl;
      exit(0);
    }
    // Parse the symbols from the elf file and build a symbol table out of them.
    BuildSymbolTable();
  }

 protected:
  std::unique_ptr<simulator::InstrumentingSimavr> simavr_;
  std::unique_ptr<simulator::UsbHost> usb_host_;
  simulator::SimulatorDelegateMock simulator_delegate_mock_;

 private:
  static void BuildSymbolTable() {
    for (int i = 0; i < firmware_.symbolcount; ++i) {
      // Demangle the symbol name.
      avr_symbol_t* symbol = *(firmware_.symbol + i);
      int status;
      char* result = abi::__cxa_demangle(symbol->symbol, 0, 0, &status);
      if (status != 0) {
        // Skip demangling failures, unless they correspond to segment-related
        // symbols and functions.
        if (absl::StartsWith(symbol->symbol, "__")) {
          symbol_table_[symbol->symbol] = SymbolInfo(symbol->addr);
        }
        continue;
      }

      // The result from __cxa_demangle will leak if not manually deallocated.
      // Convert it to a std::string here and then free it.
      std::string demangled(result);
      free(result);

      // Ignore non-standard symbol names (i.e. those that don't start with
      // `threeboard::`), such as vtable definitions or non-virtual thunks. Also
      // ignore symbols nested within anonymous namespaces.
      if (!absl::StartsWith(demangled, "threeboard::") ||
          absl::StrContains(demangled, "(anonymous namespace")) {
        continue;
      }

      // Ignore the parameter list portion of the symbol signature, and
      // everything that comes after it (CV qualifiers, for example). There's no
      // need to disambiguate between these for now.
      demangled = demangled.substr(0, demangled.find('('));
      symbol_table_[demangled] = SymbolInfo(symbol->addr);
      // Hard code the size of DelayMs since it's the only symbol whose size we
      // need right now, and its size is unlikely to change.
      // TODO: change this to use symbol->size once
      // github.com/buserror/simavr/pull/449 is merged.
      if (demangled == "threeboard::native::NativeImpl::DelayMs") {
        symbol_table_[demangled].size = 278;
      }
    }
  }

  static elf_firmware_t firmware_;
  static absl::flat_hash_map<std::string, SymbolInfo> symbol_table_;
};

elf_firmware_t IntegrationTest::firmware_;
absl::flat_hash_map<std::string, SymbolInfo> IntegrationTest::symbol_table_;

TEST_F(IntegrationTest, BootToEventLoop) {
  // Run until the threeboard has successfully started up and is running the
  // event loop. There are potentially millions of cycles here so we need to
  // set a generous timeout, InstrumentingSimavr is very slow.
  EXPECT_OK(
      simavr_->RunUntilSymbol("threeboard::Threeboard::RunEventLoopIteration",
                              std::chrono::milliseconds(3000)));
}
}  // namespace
}  // namespace integration
}  // namespace threeboard