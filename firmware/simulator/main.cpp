#include "simulator/simavr/simavr_impl.h"
#include "simulator/simulator.h"
#include "simulator/ui/ui.h"
#include "simulator/util/flags.h"
#include "simulator/util/state_storage.h"
#include "util/status_util.h"

using namespace threeboard::simulator;

absl::Status RunSimulator(int argc, char *argv[]) {
  auto firmware = std::make_unique<elf_firmware_t>();
  auto flags = Flags::ParseFromArgs(argc, argv);
  auto state_storage =
      StateStorage::CreateFromFile(flags.GetShortcutFilename());
  if (!state_storage.ok()) {
    return state_storage.status();
  }
  std::array<uint8_t, 1024> internal_eeprom_data{};
  (*state_storage)->ConfigureInternalEeprom(&internal_eeprom_data);

  auto simavr = SimavrImpl::Create(firmware.get(), &internal_eeprom_data);
  Simulator simulator(simavr.get(), state_storage->get());
  simulator.RunAsync();
  UI ui(&simulator, &flags);
  ui.Run();
  return absl::OkStatus();
}

int main(int argc, char *argv[]) { DIE_IF_ERROR(RunSimulator(argc, argv)); }
