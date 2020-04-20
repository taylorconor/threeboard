#include "bootstrap.h"
#include "native/mcu.h"

// When building the ELF binary for use in the simavr simulator, the simulator
// needs some extra information about the host MCU added to the ELF file, which
// is done using this header.
#ifdef THREEBOARD_SIM
#include "external/third_party/include/simavr/avr/avr_mcu_section.h"
AVR_MCU(F_CPU, MCU_NAME);
#endif

// From reading the .elf file, it seems like the simavr stuff is being linked in
// correctly (elf file makes reference to len, val, tag which are part of the
// struct placed in .mmcu). How else can we debug this?
// - Custom build of simavr with debug code?
// - FInd a way or program to debug elf files

int main() {
  RunThreeboard();
  return 0;
}
