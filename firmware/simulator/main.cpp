#include "simulator.h"
#include "simulator/simavr/simavr_impl.h"

using namespace threeboard::simulator;

// Firmware file path, relative to threeboard/firmware. Bazel will guarantee
// this is built since it's listed as a dependency.
const std::string kFirmwareFile =
    "simulator/components/threeboard_sim_binary.elf";

int main(int argc, char *argv[]) {
  auto simavr = SimavrImpl::Create(kFirmwareFile);
  Simulator simulator(simavr.get());
  simulator.Run();
}
