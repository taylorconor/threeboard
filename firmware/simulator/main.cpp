#include "simulator/flags.h"
#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"

using namespace threeboard::simulator;

// Firmware file path, relative to threeboard/firmware. Bazel will guarantee
// this is built since it's listed as a dependency.
const std::string kFirmwareFile =
    "simulator/components/threeboard_sim_binary.elf";

int main(int argc, char *argv[]) {
  auto simavr = SimavrImpl::Create(kFirmwareFile);
  auto flags = Flags::ParseFromArgs(argc, argv);
  Simulator simulator(&flags, simavr.get());
  simulator.Run();
}
