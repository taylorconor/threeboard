#pragma once

#include "native.h"

#include "gmock/gmock.h"

namespace threeboard {
namespace native {
namespace {
class NativeMock : public Native {
public:
  MOCK_METHOD(TimerInterruptHandlerDelegate *, GetTimerInterruptHandlerDelegate,
              (), (const override));
  MOCK_METHOD(void, SetTimerInterruptHandlerDelegate,
              (TimerInterruptHandlerDelegate *), (override));
  MOCK_METHOD(UsbInterruptHandlerDelegate *, GetUsbInterruptHandlerDelegate, (),
              (const override));
  MOCK_METHOD(void, SetUsbInterruptHandlerDelegate,
              (UsbInterruptHandlerDelegate *), (override));

  MOCK_METHOD(void, EnableInterrupts, (), (override));
  MOCK_METHOD(void, DisableInterrupts, (), (override));

  MOCK_METHOD(void, EnableTimer1, (), (override));
  MOCK_METHOD(void, EnableTimer3, (), (override));

  MOCK_METHOD(void, Delay, (uint8_t), (const override));

  MOCK_METHOD(uint16_t, ReadPgmWord, (const uint8_t *), (const override));
  MOCK_METHOD(uint8_t, ReadPgmByte, (const uint8_t *), (const override));

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

  MOCK_METHOD(void, SetUEDATX, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUEDATX, (), (override));
  MOCK_METHOD(void, SetUEINTX, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUEINTX, (), (const override));
  MOCK_METHOD(void, SetUDINT, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUDINT, (), (const override));
  MOCK_METHOD(uint8_t, GetRXOUTI, (), (const override));
  MOCK_METHOD(uint8_t, GetEORSTI, (), (const override));
  MOCK_METHOD(uint8_t, GetEPEN, (), (const override));
  MOCK_METHOD(void, SetUECONX, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUECFG0X, (), (const override));
  MOCK_METHOD(void, SetUECFG1X, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUDMFN, (), (const override));
  MOCK_METHOD(uint8_t, GetSREG, (), (const override));
  MOCK_METHOD(void, SetSREG, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUDFNUML, (), (const override));
  MOCK_METHOD(void, SetUHWCON, (const uint8_t), (override));
  MOCK_METHOD(void, SetUSBCON, (const uint8_t), (override));
  MOCK_METHOD(void, SetPLLCSR, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetPLLCSR, (), (const override));
  MOCK_METHOD(void, SetUENUM, (const uint8_t), (override));
  MOCK_METHOD(void, SetUDCON, (const uint8_t), (override));
  MOCK_METHOD(void, SetUDIEN, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUDCON, (), (const override));
  MOCK_METHOD(void, SetUECFG0X, (const uint8_t), (override));
  MOCK_METHOD(void, SetUEIENX, (const uint8_t), (override));
  MOCK_METHOD(uint8_t, GetUDADDR, (), (const override));
  MOCK_METHOD(void, SetUDADDR, (const uint8_t), (override));
  MOCK_METHOD(void, SetUERST, (const uint8_t), (override));
};
} // namespace
} // namespace native
} // namespace threeboard
