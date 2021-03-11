#pragma once

#include "gmock/gmock.h"
#include "src/delegates/error_handler_delegate.h"

namespace threeboard {
class ErrorHandlerDelegateMockDefault : public ErrorHandlerDelegate {
 public:
};

using ErrorHandlerDelegateMock =
    ::testing::StrictMock<ErrorHandlerDelegateMockDefault>;

}  // namespace threeboard
