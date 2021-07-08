#include "instrumenting_simavr.h"

#include <cstdlib>
#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "util/status_util.h"

namespace threeboard {
namespace simulator {

constexpr int kCoreDumpSize = 16;
constexpr int kRunsBetweenTimeoutCheck = 10000;

// static.
std::unique_ptr<InstrumentingSimavr> InstrumentingSimavr::Create(
    elf_firmware_t* elf_firmware,
    absl::flat_hash_map<std::string, avr_symbol_t*>* symbol_table) {
  auto avr_ptr = ParseElfFile(elf_firmware);
  auto* raw_ptr =
      new InstrumentingSimavr(elf_firmware, symbol_table, std::move(avr_ptr));
  return std::unique_ptr<InstrumentingSimavr>(raw_ptr);
}

InstrumentingSimavr::InstrumentingSimavr(
    elf_firmware_t* elf_firmware,
    absl::flat_hash_map<std::string, avr_symbol_t*>* symbol_table,
    std::unique_ptr<avr_t> avr)
    : SimavrImpl(std::move(avr), elf_firmware), symbol_table_(symbol_table) {
  data_start_ = symbol_table_->at("__data_start")->addr;
}

absl::Status InstrumentingSimavr::RunWithTimeout(
    const std::chrono::milliseconds& timeout = std::chrono::milliseconds(100)) {
  auto start = std::chrono::system_clock::now();
  while (timeout > std::chrono::system_clock::now() - start) {
    for (int i = 0; i < kRunsBetweenTimeoutCheck; ++i) {
      RETURN_IF_ERROR(RunWithIntegrityChecks());
    }
  }
  return absl::OkStatus();
}

absl::Status InstrumentingSimavr::RunUntilSymbol(
    const std::string& symbol,
    const std::chrono::milliseconds& timeout = std::chrono::milliseconds(100)) {
  if (!symbol_table_->contains(symbol)) {
    return absl::InternalError(
        absl::StrCat("Symbol '", symbol, "' not found in symbol table"));
  }
  uint32_t stop_addr = symbol_table_->at(symbol)->addr;
  auto start = std::chrono::system_clock::now();
  while (timeout > std::chrono::system_clock::now() - start) {
    for (int i = 0; i < kRunsBetweenTimeoutCheck; ++i) {
      if (avr_->pc == stop_addr) {
        return absl::OkStatus();
      }
      RETURN_IF_ERROR(RunWithIntegrityChecks());
    }
  }
  return absl::DeadlineExceededError(
      absl::StrCat("RunUntil timed out after ", timeout.count(), "ms"));
}

std::vector<uint32_t> InstrumentingSimavr::GetPrevProgramCounters() const {
  return prev_pcs_;
}

std::vector<uint16_t> InstrumentingSimavr::GetPrevStackPointers() const {
  return prev_sps_;
}

void InstrumentingSimavr::PrintCoreDump() const {
  // TODO: replace this nightmare with libfort:
  // https://github.com/seleznevae/libfort.
  std::vector<std::string> pcs = {
      absl::StrCat("0x", absl::Hex(avr_->pc, absl::kZeroPad4))};
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

  std::cout << "Dumping core from cycle " << GetCycle() << ":" << std::endl;
  std::cout << absl::StrCat("┏━━━━━━━━┯━━━━━━ CORE DUMP ━━━━━━━━━━━━━━━┓\n",
                            "┃ PC:    │ SP:    │ Register file:        ┃\n",
                            "┠────────┼────────┼───────────────────────┨\n",
                            absl::StrJoin(combined, "\n"),
                            "\n┗━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━┛")
            << std::endl;
}

void InstrumentingSimavr::Run() {
  // TODO: replace these vectors with circular buffers, since their size should
  // be fixed.
  prev_pcs_.push_back(avr_->pc);
  prev_sps_.push_back(GetStackPointer());
  SimavrImpl::Run();
  static auto& symbol = symbol_table_->at("__do_clear_bss");
  if (!finished_do_copy_data_ && symbol->addr == avr_->pc) {
    finished_do_copy_data_ = true;
  }
}

void InstrumentingSimavr::CopyDataSegment(std::vector<uint8_t>* v) const {
  v->assign(avr_->data + data_start_,
            avr_->data + data_start_ + firmware_->datasize);
}

bool InstrumentingSimavr::ShouldRunIntegrityCheckAtCurrentCycle() const {
  if (!finished_do_copy_data_) {
    return false;
  }
  static auto& symbol =
      symbol_table_->at("threeboard::native::NativeImpl::DelayMs");
  if (avr_->pc >= symbol->addr && avr_->pc <= symbol->addr + symbol->size) {
    return false;
  }
  return true;
}

absl::Status InstrumentingSimavr::RunWithIntegrityChecks() {
  if (ShouldRunIntegrityCheckAtCurrentCycle()) {
    CopyDataSegment(&data_before_);
    Run();
    // Compare the .data segment before and after run. Note that std::equal is
    // an order of magnitude slower than manually invoking memcmp here.
    auto* data_after = avr_->data + data_start_;
    if (memcmp(data_before_.data(), data_after, firmware_->datasize) != 0) {
      for (int i = 0; i < data_before_.size(); ++i) {
        if (data_before_[i] != data_after[i]) {
          std::cout << absl::StrCat(".data 0x", absl::Hex(data_start_ + i),
                                    " modified: 0x", absl::Hex(data_before_[i]),
                                    " -> 0x", absl::Hex(data_after[i]), ".")
                    << std::endl;
        }
      }
      PrintCoreDump();
      return absl::InternalError(
          absl::StrCat("Data segment modified. Core dumped."));
    }
  } else {
    Run();
  }

  uint8_t state = avr_->state;
  if (state == simulator::CRASHED || state == simulator::DONE) {
    PrintCoreDump();
    return absl::InternalError(absl::StrCat(
        "Simulator entered error state: ", state, ". Core dumped."));
  }
  if (avr_->pc == 0 && prev_pcs_.back() != 0) {
    PrintCoreDump();
    return absl::InternalError(
        absl::StrCat("Simulator unexpectedly jumped to 0x0. Core dumped."));
  }
  return absl::OkStatus();
}

}  // namespace simulator
}  // namespace threeboard