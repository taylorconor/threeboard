#pragma once

#include "simavr.h"

#include "gmock/gmock.h"

namespace threeboard {
namespace simulator {
namespace detail {

class DefaultSimavrMock : public Simavr {
public:
  MOCK_METHOD(void, Run, (), (override));

  MOCK_METHOD(void, InitGdb, (), (override));
  MOCK_METHOD(void, DeinitGdb, (), (override));
  MOCK_METHOD(void, Reset, (), (override));
  MOCK_METHOD(void, Terminate, (), (override));

  MOCK_METHOD(int, InvokeIoctl, (uint32_t, void *), (override));

  MOCK_METHOD(std::unique_ptr<Lifetime>, RegisterUsbAttachCallback,
              (UsbAttachCallback *), (override));
  MOCK_METHOD(std::unique_ptr<Lifetime>, RegisterUartOutputCallback,
              (UartOutputCallback *), (override));
  MOCK_METHOD(std::unique_ptr<Lifetime>, RegisterI2cMessageCallback,
              (I2cMessageCallback *), (override));

  MOCK_METHOD(void, RaiseI2cIrq, (uint8_t, uint32_t), (override));

  MOCK_METHOD(void, SetData, (uint8_t, uint8_t), (override));
  MOCK_METHOD(void, SetState, (uint8_t), (override));
  MOCK_METHOD(void, SetGdbPort, (uint8_t), (override));

  MOCK_METHOD(uint8_t, GetData, (uint8_t), (const override));
  MOCK_METHOD(uint8_t, GetState, (), (const override));
  MOCK_METHOD(uint8_t, GetGdbPort, (), (const override));
  MOCK_METHOD(uint64_t, GetCycle, (), (const override));
  MOCK_METHOD(uint16_t, GetStackPointer, (), (const override));
  MOCK_METHOD(uint16_t, GetBssSectionSize, (), (const override));
  MOCK_METHOD(uint16_t, GetDataSectionSize, (), (const override));
  MOCK_METHOD(uint16_t, GetRamSize, (), (const override));

  MOCK_METHOD(uint32_t, TwiIrqMsg, (uint8_t, uint8_t, uint8_t),
              (const override));
};
} // namespace detail

using SimavrMock = ::testing::StrictMock<detail::DefaultSimavrMock>;

} // namespace simulator
} // namespace threeboard