#include "testable_simavr.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "util/status_util.h"

namespace threeboard {
namespace integration {
namespace {

constexpr int kRunsBetweenTimeoutCheck = 10000;
const std::string kFirmwareFile =
    "simulator/native/threeboard_sim_fast_binary.elf";

}  // namespace

std::mutex TestableSimavr::symbol_table_mutex_;
absl::flat_hash_map<std::string, avr_symbol_t*> TestableSimavr::symbol_table_;

// static.
std::unique_ptr<TestableSimavr> TestableSimavr::Create(
    std::array<uint8_t, 1024>* internal_eeprom_data) {
  auto firmware = std::make_unique<elf_firmware_t>();
  auto avr_ptr =
      ParseElfFile(kFirmwareFile, firmware.get(), internal_eeprom_data);
  auto* raw_ptr = new TestableSimavr(std::move(avr_ptr), std::move(firmware));
  return std::unique_ptr<TestableSimavr>(raw_ptr);
}

TestableSimavr::TestableSimavr(std::unique_ptr<avr_t> avr,
                               std::unique_ptr<elf_firmware_t> elf_firmware)
    : SimavrImpl(std::move(avr), std::move(elf_firmware)) {
  std::lock_guard<std::mutex> lock(symbol_table_mutex_);
  if (symbol_table_.empty()) {
    BuildSymbolTable(firmware_.get());
  }
}

absl::Status TestableSimavr::RunWithChecks() {
  simulator::SimavrImpl::Run();
  return absl::OkStatus();
}

absl::Status TestableSimavr::RunUntilStartKeypressProcessing() {
  return RunUntilSymbol("threeboard::KeyController::PollKeyState",
                        std::chrono::milliseconds(3000));
}

absl::Status TestableSimavr::RunUntilFullLedRefresh() {
  for (int i = 0; i < 5; ++i) {
    RETURN_IF_ERROR(RunUntilSymbol("threeboard::LedController::ScanNextLine",
                                   std::chrono::milliseconds(3000)));
  }
  return absl::OkStatus();
}

absl::Status TestableSimavr::RunUntilNextEventLoopIteration() {
  return RunUntilSymbol("threeboard::Threeboard::RunEventLoopIteration",
                        std::chrono::milliseconds(3000));
}

absl::Status TestableSimavr::RunUntilSymbol(
    const std::string& symbol, const std::chrono::milliseconds& timeout) {
  if (!symbol_table_.contains(symbol)) {
    return absl::InternalError(
        absl::StrCat("Symbol '", symbol, "' not found in symbol table"));
  }
  uint32_t stop_addr = symbol_table_.at(symbol)->addr;
  auto start = std::chrono::system_clock::now();
  while (timeout > std::chrono::system_clock::now() - start) {
    for (int i = 0; i < kRunsBetweenTimeoutCheck; ++i) {
      RETURN_IF_ERROR(RunWithChecks());
      if (avr_->pc == stop_addr) {
        return absl::OkStatus();
      }
    }
  }
  return absl::DeadlineExceededError(
      absl::StrCat("RunUntil timed out after ", timeout.count(),
                   "ms for symbol '", symbol, "'"));
}

// Static.
void TestableSimavr::BuildSymbolTable(elf_firmware_t* firmware) {
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
    // ignore symbols nested within anonymous namespaces, and symbols
    // identifying static variables. This is clearly not an exact science but it
    // will do unless we start experiencing problems with symbol parsing.
    if (!absl::StartsWith(demangled, "threeboard::") ||
        absl::StrContains(demangled, "(anonymous namespace") ||
        absl::StrContains(demangled, ")::")) {
      continue;
    }

    // Ignore the parameter list portion of the symbol signature, and
    // everything that comes after it (CV qualifiers, for example). There's no
    // need to disambiguate between these for now.
    demangled = demangled.substr(0, demangled.find('('));
    symbol_table_[demangled] = symbol;
  }
}

}  // namespace integration
}  // namespace threeboard
