#pragma once

#include "request_handler.h"

#include "gmock/gmock.h"

namespace threeboard {
namespace usb {

class RequestHandlerMock : public RequestHandler {
public:
  RequestHandlerMock(native::Native *native) : RequestHandler(native) {}

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
} // namespace usb
} // namespace threeboard
