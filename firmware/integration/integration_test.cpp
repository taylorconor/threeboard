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

namespace threeboard {
namespace integration {
namespace {

class IntegrationTest : public testing::Test {
 public:
  IntegrationTest() {
    std::unique_ptr<elf_firmware_t> firmware =
        std::make_unique<elf_firmware_t>();
    if (elf_read_firmware("simulator/native/threeboard_sim_binary.elf",
                          firmware.get())) {
      std::cout << "Failed to parse threeboard ELF file for simulated testing"
                << std::endl;
      exit(0);
    }
    if (symbol_table_.empty()) {
      BuildSymbolTable(firmware.get());
    }
    simavr_ = simulator::InstrumentingSimavr::Create(
        std::move(firmware), &internal_eeprom_data_, &symbol_table_);
    usb_host_ = std::make_unique<simulator::UsbHost>(simavr_.get(),
                                                     &simulator_delegate_mock_);
  }

 protected:
  std::array<uint8_t, 1024> internal_eeprom_data_{};
  std::unique_ptr<simulator::InstrumentingSimavr> simavr_;
  std::unique_ptr<simulator::UsbHost> usb_host_;
  simulator::SimulatorDelegateMock simulator_delegate_mock_;

 private:
  static void BuildSymbolTable(elf_firmware_t* firmware) {
    for (int i = 0; i < firmware->symbolcount; ++i) {
      // Demangle the symbol name.
      avr_symbol_t* symbol = *(firmware->symbol + i);
      int status;
      char* result = abi::__cxa_demangle(symbol->symbol, 0, 0, &status);
      if (status != 0) {
        // Skip demangling failures, unless they correspond to segment-related
        // symbols and functions.
        if (absl::StartsWith(symbol->symbol, "__")) {
          symbol_table_[symbol->symbol] = symbol;
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
      symbol_table_[demangled] = symbol;
    }
  }

  static absl::flat_hash_map<std::string, avr_symbol_t*> symbol_table_;
};

absl::flat_hash_map<std::string, avr_symbol_t*> IntegrationTest::symbol_table_;

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