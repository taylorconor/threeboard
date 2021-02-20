#pragma once

#include "gmock/gmock.h"
#include "src/delegates/keypress_handler_delegate.h"

namespace threeboard {
class KeypressHandlerDelegateMockDefault : public KeypressHandlerDelegate {
 public:
  MOCK_METHOD(void, HandleKeypress, (const Keypress &), (override));
};

using KeypressHandlerDelegateMock =
    ::testing::StrictMock<KeypressHandlerDelegateMockDefault>;

}  // namespace threeboard
