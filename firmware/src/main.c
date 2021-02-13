#include "src/bootstrap.h"

// When building the ELF binary for use in the simavr simulator, the simulator
// needs some extra information about the host MCU added to the ELF file, which
// is done using this header.
#ifdef THREEBOARD_SIM
#include "native/mcu.h"

// TODO: make this include relative to simavr, not external/third_party. Right
// now it's like this because of limitations in simavr_avr_hdrs which I don't
// have the time / the will to fix at the moment.
#include "external/third_party/local/include/simavr/avr/avr_mcu_section.h"
AVR_MCU(F_CPU, MCU_NAME);
#endif

// This main function is just a C wrapper around the C++ `bootstrap.cpp`. The
// reason we do this is to enable the raw ELF file produced by building this
// main file to be testable in the simavr simulator. The simulator relies on
// linking in a section to the ELF file describing the MCU settings (name and
// frequency) defined by AVR_MCU above. This macro produces code that is not
// syntactically compatible with C++.
int main() {
  // Forcibly disable all dynamic memory allocation by restricting malloc's heap
  // space size to 0. Everything in the threeboard is statically allocated,
  // with the memory space split between .data (initialised static variables),
  // .bss (uninitialised static variables), and the remainder dedicated to the
  // stack.
  extern void *__malloc_heap_start;
  extern void *__malloc_heap_end;
  __malloc_heap_end = 0;
  __malloc_heap_start = 0;

  // Call into an `extern "C"` function in C++ to run the threeboard firmware.
  RunThreeboard();
  return 0;
}
