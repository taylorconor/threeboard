#include <iostream>

#include "simavr/sim_avr.h"
#include "simavr/sim_elf.h"

int main() {
  elf_firmware_t f;
  const char *fname = "simulator/threeboard_sim_binary.elf";
  if (elf_read_firmware(fname, &f)) {
    std::cout << "Failed to read ELF firmware '" << fname << "'" << std::endl;
    exit(1);
  }
  std::cout << "Loaded firmware " << fname << ", frequency = " << f.frequency
            << ", mmcu = " << f.mmcu << ", vcc = " << f.vcc
            << ", avcc = " << f.avcc << ", aref = " << f.aref
            << ", tracename = " << f.tracename << std::endl;
  avr_t *avr = avr_make_mcu_by_name(f.mmcu);
  if (!avr) {
    std::cout << "AVR '" << f.mmcu << "' not known" << std::endl;
    exit(1);
  }
  avr_init(avr);
  avr_load_firmware(avr, &f);
}
