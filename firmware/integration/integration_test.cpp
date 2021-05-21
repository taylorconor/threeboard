#include "gtest/gtest.h"
#include "integration/simulated_test_base.h"

namespace threeboard {
namespace integration {
namespace {

class IntegrationTest : public integration::SimulatedTestBase {
 public:
  IntegrationTest() {}
};

TEST_F(IntegrationTest, FakeTest) {}
}  // namespace
}  // namespace integration
}  // namespace threeboard