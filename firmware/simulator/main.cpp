#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/util/flags.h"

using namespace threeboard::simulator;

int main(int argc, char *argv[]) {
  auto simavr = SimavrImpl::Create();
  auto flags = Flags::ParseFromArgs(argc, argv);
  Simulator simulator(&flags, simavr.get());
  simulator.Run();
}
