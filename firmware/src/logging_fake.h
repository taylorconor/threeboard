#pragma once

#include "src/logging.h"
#include "src/native/native_mock.h"

namespace threeboard {

class LoggingFake {
 public:
  LoggingFake() {
    Logging::Init(&native_mock_);
    EXPECT_CALL(native_mock_, SetUDR1(testing::_))
        .WillRepeatedly(testing::Return());
  }

 private:
  native::NativeMock native_mock_;
};
}  // namespace threeboard