#include <cxxabi.h>

#include <chrono>
#include <unordered_map>

#include "absl/status/status.h"
#include "gtest/gtest.h"
#include "integration/util/instrumenting_simavr.h"
#include "simavr/sim_elf.h"
#include "simulator/components/usb_host.h"
#include "simulator/simulator_delegate_mock.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

class IntegrationTest : public testing::Test {
 public:
  IntegrationTest() {
    std::unique_ptr<elf_firmware_t> firmware =
        std::make_unique<elf_firmware_t>();
    if (elf_read_firmware("simulator/native/threeboard_sim_binary.elf",
                          firmware.get())) {
      std::cout << "Failed to parse threeboard ELF file for simulated testing"
                << std::endl;
      exit(0);
    }
    simavr_ = simulator::InstrumentingSimavr::Create(std::move(firmware),
                                                     &internal_eeprom_data_);
    usb_host_ = std::make_unique<simulator::UsbHost>(simavr_.get(),
                                                     &simulator_delegate_mock_);
  }

 protected:
  std::array<uint8_t, 1024> internal_eeprom_data_{};
  std::unique_ptr<simulator::InstrumentingSimavr> simavr_;
  std::unique_ptr<simulator::UsbHost> usb_host_;
  simulator::SimulatorDelegateMock simulator_delegate_mock_;
};

TEST_F(IntegrationTest, BootToEventLoop) {
  // Run until the threeboard has successfully started up and is running the
  // event loop. There are potentially millions of cycles here so we need to
  // set a generous timeout, InstrumentingSimavr is very slow.
  EXPECT_OK(
      simavr_->RunUntilSymbol("threeboard::Threeboard::RunEventLoopIteration",
                              std::chrono::milliseconds(3000)));
}
}  // namespace
}  // namespace integration
}  // namespace threeboard