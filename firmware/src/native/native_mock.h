#pragma once

#include "native.h"

#include "gmock/gmock.h"

namespace threeboard {
namespace native {
namespace {
class NativeMock : public Native {
public:
  MOCK_METHOD(void, SetTimer1InterruptHandler, (InterruptHandlerDelegate *),
              (override));
  MOCK_METHOD(void, SetTimer3InterruptHandler, (InterruptHandlerDelegate *),
              (override));
  MOCK_METHOD(InterruptHandlerDelegate *, GetInterruptHandlerDelegate, (),
              (const, override));

  MOCK_METHOD(void, Delay, (uint8_t), (const, override));

  MOCK_METHOD(void, EnableDDRB, (const uint8_t), (override));
  MOCK_METHOD(void, DisableDDRB, (const uint8_t), (override));
  MOCK_METHOD(void, EnableDDRD, (const uint8_t), (override));
  MOCK_METHOD(void, EnableDDRF, (const uint8_t), (override));

  MOCK_METHOD(void, EnablePORTB, (const uint8_t), (override));
  MOCK_METHOD(void, DisablePORTB, (const uint8_t), (override));
  MOCK_METHOD(void, EnablePORTD, (const uint8_t), (override));
  MOCK_METHOD(void, DisablePORTD, (const uint8_t), (override));
  MOCK_METHOD(void, EnablePORTF, (const uint8_t), (override));
  MOCK_METHOD(void, DisablePORTF, (const uint8_t), (override));

  MOCK_METHOD(uint8_t, GetPINB, (), (const, override));
};
} // namespace
} // namespace native
} // namespace threeboard
