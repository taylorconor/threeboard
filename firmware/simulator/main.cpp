#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/ui/ui.h"
#include "simulator/util/flags.h"
#include "simulator/util/state_storage.h"
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
  auto state_storage =
      StateStorage::CreateFromFile(flags.GetShortcutFilename());
  if (!state_storage.ok()) {
    return state_storage.status();
  }
  std::array<uint8_t, 1024> internal_eeprom_data{};
  (*state_storage)->ConfigureInternalEeprom(&internal_eeprom_data);
  auto simavr = SimavrImpl::Create(&internal_eeprom_data);

  Simulator simulator(simavr.get(), state_storage->get());
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
