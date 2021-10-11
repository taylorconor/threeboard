#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/ui/ui.h"
#include "simulator/util/flags.h"
#include "simulator/util/state_storage.h"
#include "util/status_util.h"

using namespace threeboard::simulator;

absl::Status RunSimulator(int argc, char *argv[]) {
  auto flags = Flags::ParseFromArgs(argc, argv);
  ASSIGN_OR_RETURN(auto state_storage,
                   StateStorage::CreateFromFile(flags.GetShortcutFilename()));
  auto simavr = SimavrImpl::Create(state_storage->GetInternalEepromData());

  Simulator simulator(simavr.get(), state_storage.get());
  UI ui(&simulator, &flags);
  ui.Run();
  return absl::OkStatus();
}

int main(int argc, char *argv[]) {
  DIE_IF_ERROR(RunSimulator(argc, argv));
  std::cout << "Simulator shut down successfully." << std::endl;
}
