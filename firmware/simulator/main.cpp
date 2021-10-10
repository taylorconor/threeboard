#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/ui/ui.h"
#include "simulator/util/flags.h"
#include "simulator/util/state_storage.h"
#include "util/status_util.h"

using namespace threeboard::simulator;

absl::Status RunSimulator(int argc, char *argv[]) {
  auto flags = Flags::ParseFromArgs(argc, argv);
  auto state_storage_or =
      StateStorage::CreateFromFile(flags.GetShortcutFilename());
  StateStorage *state_storage = nullptr;
  if (state_storage_or.ok()) {
    state_storage = state_storage_or->get();
    std::cout << "Populating EEPROM data from storage file: "
              << flags.GetShortcutFilename() << std::endl;
  } else {
    std::cout
        << "No shortcut storage file specified, not populating EEPROM data"
        << std::endl;
  }
  std::array<uint8_t, 1024> internal_eeprom_data{};
  if (state_storage) {
    state_storage->ConfigureInternalEeprom(&internal_eeprom_data);
  }
  auto simavr = SimavrImpl::Create(&internal_eeprom_data);

  Simulator simulator(simavr.get(), state_storage);
  UI ui(&simulator, &flags);
  ui.Run();
  return absl::OkStatus();
}

int main(int argc, char *argv[]) {
  DIE_IF_ERROR(RunSimulator(argc, argv));
  std::cout << "Simulator shut down successfully." << std::endl;
}
