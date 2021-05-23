#include "gtest/gtest.h"
#include "integration/simulated_test_base.h"
#include "util/gtest_util.h"

namespace threeboard {
namespace integration {
namespace {

class IntegrationTest : public integration::SimulatedTestBase {
 public:
  IntegrationTest() {}
};

TEST_F(IntegrationTest, BootToEventLoop) {
  // Run until the threeboard has successfully started up and is running the
  // event loop. This means it has already run the 250ms blocking boot sequence
  // indicator, so we set the timeout generously here to avoid flakiness.
  EXPECT_OK(RunUntil("threeboard::Threeboard::RunEventLoopIteration",
                     std::chrono::milliseconds(500)));
}
}  // namespace
}  // namespace integration
}  // namespace threeboard