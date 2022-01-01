#include "instrumenting_simavr.h"

#include <cstdlib>
#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "util/status_util.h"

namespace threeboard {
namespace integration {
namespace {

constexpr int kCoreDumpSize = 16;

const std::string kFirmwareFile =
    "simulator/native/threeboard_sim_fast_binary.elf";
}  // namespace

// static.
std::unique_ptr<InstrumentingSimavr> InstrumentingSimavr::Create(
    std::array<uint8_t, 1024>* internal_eeprom_data) {
  auto firmware = std::make_unique<elf_firmware_t>();
  auto avr_ptr =
      ParseElfFile(kFirmwareFile, firmware.get(), internal_eeprom_data);
  auto* raw_ptr =
      new InstrumentingSimavr(std::move(avr_ptr), std::move(firmware));
  return std::unique_ptr<InstrumentingSimavr>(raw_ptr);
}

absl::Status InstrumentingSimavr::RunWithChecks() {
  bool should_check = ShouldRunIntegrityCheckAtCurrentCycle();
  if (should_check) {
    CopyDataSegment(&data_before_);
  }
  prev_pcs_.push_back(avr_->pc);
  prev_sps_.push_back(GetStackPointer());
  Run();
  static auto& symbol = symbol_table_.at("__do_clear_bss");
  if (!finished_do_copy_data_ && symbol->addr == avr_->pc) {
    finished_do_copy_data_ = true;
  }
  if (should_check) {
    // Compare the .data segment before and after run. Note that std::equal is
    // an order of magnitude slower than manually invoking memcmp here.
    auto* data_after = avr_->data + data_start_;
    if (memcmp(data_before_.data(), data_after, firmware_->datasize) != 0) {
      for (int i = 0; i < data_before_.size(); ++i) {
        if (data_before_[i] != data_after[i]) {
          std::cout << absl::StrCat(".data 0x", absl::Hex(data_start_ + i),
                                    " modified at PC 0x: 0x",
                                    absl::Hex(avr_->pc), ": ",
                                    absl::Hex(data_before_[i]), " -> 0x",
                                    absl::Hex(data_after[i]), ".")
                    << std::endl;
        }
      }
      PrintCoreDump();
      return absl::InternalError(
          absl::StrCat("Data segment modified. Core dumped."));
    }
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

InstrumentingSimavr::InstrumentingSimavr(
    std::unique_ptr<avr_t> avr, std::unique_ptr<elf_firmware_t> elf_firmware)
    : TestableSimavr(std::move(avr), std::move(elf_firmware)) {
  if (symbol_table_.empty()) {
    BuildSymbolTable(firmware_.get());
  }
  data_start_ = symbol_table_.at("__data_start")->addr;
}

void InstrumentingSimavr::PrintCoreDump() const {
  // TODO: Consider replacing this with libfort:
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
        (i == 0 ? "┃>" : "┃ "), (pcs[i].empty() ? "      " : pcs[i]), " │ ",
        (sps[i].empty() ? "      " : sps[i]), " │ r", i, (i > 9 ? ": " : ":  "),
        "0x", absl::Hex(GetData(i), absl::kZeroPad2), " | r", i + 16, ": 0x",
        absl::Hex(GetData(i + 16), absl::kZeroPad2), " ┃"));
  }

  std::cout << "Dumping core from cycle " << GetCycle() << ":" << std::endl;
  std::cout << absl::StrCat("┏━━━━━━━━┯━━━━━━ CORE DUMP ━━━━━━━━━━━━━━━┓\n",
                            "┃ PC:    │ SP:    │ Register file:        ┃\n",
                            "┠────────┼────────┼───────────────────────┨\n",
                            absl::StrJoin(combined, "\n"),
                            "\n┗━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━┛")
            << std::endl;
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
      symbol_table_.at("threeboard::native::NativeImpl::DelayMs");
  if (avr_->pc >= symbol->addr && avr_->pc <= symbol->addr + symbol->size) {
    return false;
  }
  return true;
}

}  // namespace integration
}  // namespace threeboard