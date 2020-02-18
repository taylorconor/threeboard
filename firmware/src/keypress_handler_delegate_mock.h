#pragma once

#include "keypress_handler_delegate.h"

#include "gmock/gmock.h"

namespace threeboard {
class KeypressHandlerDelegateMock : public KeypressHandlerDelegate {
public:
  MOCK_METHOD(void, HandleKeypress, (const Keypress), (override));
};
} // namespace threeboard
