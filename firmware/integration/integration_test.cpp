#include "gtest/gtest.h"
#include "integration/simulated_test_base.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

class IntegrationTest : public SimulatedTestBase {
 public:
  IntegrationTest() {}
};

TEST_F(IntegrationTest, BootToEventLoop) {
  // Run until the threeboard has successfully started up and is running the
  // event loop. There are potentially millions of cycles here so we need to set
  // a generous timeout, InstrumentingSimavr is very slow.
  EXPECT_OK(
      simavr_->RunUntilSymbol("threeboard::Threeboard::RunEventLoopIteration",
                              std::chrono::milliseconds(3000)));
}
}  // namespace
}  // namespace integration
}  // namespace threeboard