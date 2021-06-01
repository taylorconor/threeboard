#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/util/flags.h"

using namespace threeboard::simulator;

int main(int argc, char *argv[]) {
  auto firmware = std::make_unique<elf_firmware_t>();
  auto simavr = SimavrImpl::Create(firmware.get());
  auto flags = Flags::ParseFromArgs(argc, argv);
  Simulator simulator(&flags, simavr.get());
  simulator.Run();
}
