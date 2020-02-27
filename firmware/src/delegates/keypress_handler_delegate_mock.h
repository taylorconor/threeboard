#pragma once

#include "keypress_handler_delegate.h"

#include "gmock/gmock.h"

namespace threeboard {
class KeypressHandlerDelegateMockDefault : public KeypressHandlerDelegate {
public:
  MOCK_METHOD(void, HandleKeypress, (const Keypress), (override));
};

using KeypressHandlerDelegateMock =
    ::testing::StrictMock<KeypressHandlerDelegateMockDefault>;

} // namespace threeboard
