#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/ui/ui.h"
#include "simulator/util/flags.h"
#include "simulator/util/state_storage_impl.h"
#include "src/keypress.h"
#include "util/status_util.h"

using namespace threeboard::simulator;
using threeboard::Keypress;

std::vector<char> GetKeycodes(const Keypress &keypress) {
  switch (keypress) {
    case Keypress::X:
      return {'a'};
    case Keypress::Y:
      return {'s'};
    case Keypress::Z:
      return {'d'};
    case Keypress::XY:
      return {'a', 's'};
    case Keypress::XZ:
      return {'a', 'd'};
    case Keypress::YZ:
      return {'s', 'd'};
    case Keypress::XYZ:
      return {'a', 's', 'd'};
    default:
      return {};
  }
}

absl::Status RunSimulator(int argc, char *argv[]) {
  auto flags = Flags::ParseFromArgs(argc, argv);
  ASSIGN_OR_RETURN(auto state_storage, StateStorageImpl::CreateFromFile(
                                           flags.GetShortcutFilename()));
  auto simavr = SimavrImpl::Create(state_storage->GetInternalEepromData());

  Simulator simulator(simavr.get(), state_storage.get());
  UI ui(&simulator, &flags);
  simulator.EnableLogging(&ui);

  simulator.RunAsync();
  ui.Run();
  return absl::OkStatus();
}

int main(int argc, char *argv[]) {
  DIE_IF_ERROR(RunSimulator(argc, argv));
  std::cout << "Simulator shut down successfully." << std::endl;
}
