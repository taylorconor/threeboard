#include "simavr_for_testing.h"

#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "util/status_util.h"

namespace threeboard {
namespace simulator {

constexpr int kCoreDumpSize = 16;

// static.
std::unique_ptr<SimavrForTesting> SimavrForTesting::Create(
    absl::flat_hash_map<std::string, uint32_t>* symbol_table) {
  elf_firmware_t f;
  auto avr_ptr = ParseElfFile(&f);
  auto* raw_ptr = new SimavrForTesting(symbol_table, std::move(avr_ptr),
                                       f.bsssize, f.datasize);
  return std::unique_ptr<SimavrForTesting>(raw_ptr);
}

SimavrForTesting::SimavrForTesting(
    absl::flat_hash_map<std::string, uint32_t>* symbol_table,
    std::unique_ptr<avr_t> avr, uint16_t bss_size, uint16_t data_size)
    : SimavrImpl(std::move(avr), bss_size, data_size),
      symbol_table_(symbol_table) {}

absl::Status SimavrForTesting::RunWithTimeout(
    const std::chrono::milliseconds& timeout = std::chrono::milliseconds(100)) {
  auto start = std::chrono::system_clock::now();
  while (timeout > std::chrono::system_clock::now() - start) {
    Run();
    RETURN_IF_ERROR(VerifyState());
  }
  return absl::OkStatus();
}

absl::Status SimavrForTesting::RunUntilSymbol(
    const std::string& symbol,
    const std::chrono::milliseconds& timeout = std::chrono::milliseconds(100)) {
  if (!symbol_table_->contains(symbol)) {
    return absl::InternalError(
        absl::StrCat("Symbol '", symbol, "' not found in symbol table"));
  }
  uint32_t stop_addr = symbol_table_->at(symbol);
  auto start = std::chrono::system_clock::now();
  while (timeout > std::chrono::system_clock::now() - start) {
    if (GetProgramCounter() == stop_addr) {
      return absl::OkStatus();
    }

    Run();
    RETURN_IF_ERROR(VerifyState());
  }
  return absl::DeadlineExceededError(
      absl::StrCat("RunUntil timed out after ", timeout.count(), "ms"));
}

std::vector<uint32_t> SimavrForTesting::GetPrevProgramCounters() const {
  return prev_pcs_;
}

std::vector<uint16_t> SimavrForTesting::GetPrevStackPointers() const {
  return prev_sps_;
}

void SimavrForTesting::PrintCoreDump() const {
  // TODO: replace this nightmare with libfort:
  // https://github.com/seleznevae/libfort.
  std::vector<std::string> pcs = {
      absl::StrCat("0x", absl::Hex(GetProgramCounter(), absl::kZeroPad4))};
  for (int i = prev_pcs_.size(); i >= prev_pcs_.size() - kCoreDumpSize; --i) {
    pcs.push_back(
        absl::StrCat("0x", absl::Hex(prev_pcs_[i - 1], absl::kZeroPad4)));
  }

  std::vector<std::string> sps = {
      absl::StrCat("0x", absl::Hex(GetStackPointer(), absl::kZeroPad4))};
  for (int i = prev_sps_.size(); i >= prev_sps_.size() - kCoreDumpSize; --i) {
    sps.push_back(
        absl::StrCat("0x", absl::Hex(prev_sps_[i - 1], absl::kZeroPad4)));
  }

  std::vector<std::string> combined;
  for (int i = 0; i < kCoreDumpSize; ++i) {
    combined.push_back(absl::StrCat(
        (i == 0 ? "┃>" : "┃ "), pcs[i], " │ ", sps[i], " │ r", i,
        (i > 9 ? ": " : ":  "), "0x", absl::Hex(GetData(i), absl::kZeroPad2),
        " | r", i + 16, ": 0x", absl::Hex(GetData(i + 16), absl::kZeroPad2),
        " ┃"));
  }

  std::cout << absl::StrCat("┏━━━━━━━━┯━━━━━━ CORE DUMP ━━━━━━━━━━━━━━━┓\n",
                            "┃ PC:    │ SP:    │ Register file:        ┃\n",
                            "┠────────┼────────┼───────────────────────┨\n",
                            absl::StrJoin(combined, "\n"),
                            "\n┗━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━┛")
            << std::endl;
}

void SimavrForTesting::Run() {
  prev_pcs_.push_back(GetProgramCounter());
  prev_sps_.push_back(GetStackPointer());
  SimavrImpl::Run();
}

absl::Status SimavrForTesting::VerifyState() const {
  uint8_t state = GetState();
  if (state == simulator::CRASHED || state == simulator::DONE) {
    PrintCoreDump();
    return absl::InternalError(absl::StrCat(
        "Simulator entered error state: ", state, ". Core dumped."));
  }
  if (GetProgramCounter() == 0 && prev_pcs_.back() != 0) {
    PrintCoreDump();
    return absl::InternalError(
        absl::StrCat("Simulator unexpectedly jumped to 0x0. Core dumped."));
  }
  return absl::OkStatus();
}

}  // namespace simulator
}  // namespace threeboard