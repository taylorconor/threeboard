#include "firmware.h"

#include "gtest/gtest.h"
#include "simulator/simavr/simavr_mock.h"

namespace threeboard {
namespace simulator {
namespace {

using ::testing::_;
using ::testing::Return;

class FirmwareTest : public ::testing::Test {
 public:
  FirmwareTest() {
    EXPECT_CALL(simavr_mock_, Terminate()).Times(1);
    firmware_ = std::make_unique<Firmware>(&simavr_mock_);
  }

  SimavrMock simavr_mock_;
  std::unique_ptr<Firmware> firmware_;
};

// TODO: write tests!
TEST_F(FirmwareTest, FakeTest) {}
}  // namespace
}  // namespace simulator
}  // namespace threeboard