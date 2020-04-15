#include <filesystem>
#include <iostream>

#include "simavr/sim_avr.h"
#include "simavr/sim_elf.h"

int main() {
  elf_firmware_t f;
  const char *fname = "src/threeboard_bin.elf";
  std::cout << "Firmware pathname is: " << fname << std::endl;

  elf_read_firmware(fname, &f);
  std::cout << "Firmware " << fname << ", freq = " << f.frequency
            << ", mmcu = " << f.mmcu << std::endl;
}
