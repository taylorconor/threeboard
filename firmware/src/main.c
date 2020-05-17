#include "bootstrap.h"
#include "native/mcu.h"

// When building the ELF binary for use in the simavr simulator, the simulator
// needs some extra information about the host MCU added to the ELF file, which
// is done using this header.
#ifdef THREEBOARD_SIM
// TODO: make this include relative to simavr, not external/third_party.
#include "external/third_party/include/simavr/avr/avr_mcu_section.h"
AVR_MCU(F_CPU, MCU_NAME);
#endif

// This main function is so simple because main.c is a pure C file, rather than
// C++. The reason we do this is to enable the raw ELF file produced by building
// this main file to be testable in the simavr simulator. The simulator relies
// on linking in a section to the ELF file describing the MCU settings (name and
// frequency) defined by AVR_MCU above. This macro is unfortunately only
// available in C.
int main() {
  // Call into an extern "C" function in C++ too run the threeboard firmware.
  RunThreeboard();
  return 0;
}
