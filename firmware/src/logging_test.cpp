#include "logging.h"

#include <memory>

#include "src/native/native_mock.h"

using ::testing::Sequence;

namespace threeboard {
namespace {

class LoggingTest : public ::testing::Test {
 public:
  LoggingTest() { Logging::Init(&native_mock_); }

  void SetUartExpectation(const std::string &str) {
    Sequence seq;
    for (const char c : str) {
      EXPECT_CALL(native_mock_, SetUDR1(c)).Times(1).InSequence(seq);
    }
    EXPECT_CALL(native_mock_, SetUDR1('\n')).Times(1).InSequence(seq);
  }

  native::NativeMock native_mock_;
};

TEST_F(LoggingTest, TransmitLogOverUart) {
  SetUartExpectation("Test log string!");
  Logging::Log("Test log string!");
}

TEST_F(LoggingTest, CorrectlyFormatLogString) {
  SetUartExpectation("Test string: one 1");
  Logging::Log("Test string: %s %d", "one", 1);
}
}  // namespace
}  // namespace threeboard