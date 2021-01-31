#pragma once

#include "simavr.h"
#include "simavr/sim_irq.h"

#include <memory>

extern "C" {
struct avr_t;
}

namespace threeboard {
namespace simulator {

// Concrete implementation of the Simavr interface. This is the only class with
// a dependency on the simavr library.
class SimavrImpl : public Simavr {
public:
  static std::unique_ptr<Simavr> Create(const std::string &);

  ~SimavrImpl() override = default;

  void Run() override;

  void InitGdb() override;
  void DeinitGdb() override;
  void Reset() override;
  void Terminate() override;

  int InvokeIoctl(uint32_t ioctl, void *param) override;

  std::unique_ptr<Lifetime>
  RegisterUsbAttachCallback(UsbAttachCallback *callback) override;
  std::unique_ptr<Lifetime>
  RegisterUartOutputCallback(UartOutputCallback *callback) override;
  std::unique_ptr<Lifetime>
  RegisterI2cMessageCallback(I2cMessageCallback *callback) override;

  void RaiseI2cIrq(uint8_t direction, uint32_t value) override;

  void SetData(uint8_t idx, uint8_t val) override;
  void SetState(uint8_t val) override;
  void SetGdbPort(uint8_t val) override;

  uint8_t GetData(uint8_t idx) const override;
  uint8_t GetState() const override;
  uint8_t GetGdbPort() const override;
  uint64_t GetCycle() const override;
  uint16_t GetStackPointer() const override;
  uint16_t GetBssSectionSize() const override;
  uint16_t GetDataSectionSize() const override;
  uint16_t GetRamSize() const override;

private:
  SimavrImpl(std::unique_ptr<avr_t> avr, uint16_t bss_size, uint16_t data_size);

  std::unique_ptr<avr_t> avr_;
  uint16_t bss_size_;
  uint16_t data_size_;
  avr_irq_t *i2c_irq_;
};
} // namespace simulator
} // namespace threeboard