#include "simavr_for_testing.h"

#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

namespace threeboard {
namespace simulator {

// static.
std::unique_ptr<SimavrForTesting> SimavrForTesting::Create() {
  elf_firmware_t f;
  auto avr_ptr = ParseElfFile(&f);
  auto *raw_ptr =
      new SimavrForTesting(std::move(avr_ptr), f.bsssize, f.datasize);
  return std::unique_ptr<SimavrForTesting>(raw_ptr);
}

SimavrForTesting::SimavrForTesting(std::unique_ptr<avr_t> avr,
                                   uint16_t bss_size, uint16_t data_size)
    : SimavrImpl(std::move(avr), bss_size, data_size) {}

void SimavrForTesting::Run() {
  prev_pcs_.push_back(GetProgramCounter());
  prev_sps_.push_back(GetStackPointer());
  SimavrImpl::Run();
}

std::vector<uint32_t> SimavrForTesting::GetPrevProgramCounters() const {
  return prev_pcs_;
}

std::vector<uint16_t> SimavrForTesting::GetPrevStackPointers() const {
  return prev_sps_;
}

void SimavrForTesting::PrintCoreDump() const {
  const int size = 15;
  std::vector<std::string> pcs = {
      absl::StrCat(" 0x", absl::Hex(GetProgramCounter()))};
  int pc_max_len = pcs[0].length();
  for (int i = prev_pcs_.size() - 1; i >= prev_pcs_.size() - size; --i) {
    auto prev_pc = absl::StrCat(" 0x", absl::Hex(prev_pcs_[i]));
    if (prev_pc.length() > pc_max_len) {
      pc_max_len = prev_pc.length();
    }
    pcs.push_back(prev_pc);
  }

  std::vector<std::string> sps = {
      absl::StrCat(" 0x", absl::Hex(GetStackPointer()))};

  for (int i = prev_sps_.size() - 1; i >= prev_sps_.size() - size; --i) {
    sps.push_back(absl::StrCat(" 0x", absl::Hex(prev_sps_[i])));
  }

  std::vector<std::string> combined;
  for (int i = 0; i < pcs.size(); ++i) {
    combined.push_back(absl::StrCat(
        (i == 0 ? ">" : " "), pcs[i],
        std::string(pc_max_len - pcs[i].length(), ' '), " | ", sps[i]));
  }

  std::cout << absl::StrCat("=== Core dump ===\n",
                            " PC:", std::string(pc_max_len - 3, ' '),
                            " | SP:\n", absl::StrJoin(combined, "\n"),
                            "\n=================")
            << std::endl;
}

}  // namespace simulator
}  // namespace threeboard