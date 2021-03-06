#pragma once

#include "gmock/gmock.h"
#include "src/delegates/event_handler_delegate.h"

namespace threeboard {
class EventHandlerDelegateMockDefault : public EventHandlerDelegate {
 public:
  MOCK_METHOD(void, HandleKeypress, (const Keypress &), (override));
};

using EventHandlerDelegateMock =
    ::testing::StrictMock<EventHandlerDelegateMockDefault>;

}  // namespace threeboard
