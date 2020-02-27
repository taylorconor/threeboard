#pragma once

#include "request_handler.h"

#include "gmock/gmock.h"

namespace threeboard {
namespace usb {

class RequestHandlerMockDefault : public RequestHandler {
public:
  RequestHandlerMockDefault(native::Native *native) : RequestHandler(native) {}

  MOCK_METHOD(void, HandleGetStatus, (), (override));
  MOCK_METHOD(void, HandleSetAddress, (const SetupPacket &), (override));
  MOCK_METHOD(void, HandleGetDescriptor, (const SetupPacket &), (override));
  MOCK_METHOD(void, HandleGetConfiguration, (const HidState &), (override));
  MOCK_METHOD(void, HandleSetConfiguration, (const SetupPacket &, HidState *),
              (override));
  MOCK_METHOD(void, HandleGetReport, (const HidState &), (override));
  MOCK_METHOD(void, HandleGetIdle, (const HidState &), (override));
  MOCK_METHOD(void, HandleSetIdle, (const SetupPacket &, HidState *),
              (override));
  MOCK_METHOD(void, HandleGetProtocol, (const HidState &), (override));
  MOCK_METHOD(void, HandleSetProtocol, (const SetupPacket &, HidState *),
              (override));
};

using RequestHandlerMock = ::testing::StrictMock<RequestHandlerMockDefault>;

} // namespace usb
} // namespace threeboard
