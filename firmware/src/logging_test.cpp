#include "logging.h"

#include <memory>

#include "src/native/native_mock.h"

namespace threeboard {
namespace {

class LoggingTest : public ::testing::Test {
 public:
  native::NativeMock native_mock_;
};

TEST_F(LoggingTest, FakeTest) {}

}  // namespace
}  // namespace threeboard