#pragma once

#include <iostream>
#include <memory>

#include "simavr.h"
#include "simavr/sim_elf.h"
#include "simavr/sim_irq.h"

extern "C" {
struct avr_t;
}

namespace threeboard {
namespace simulator {

// Concrete implementation of the Simavr interface. This is the only class with
// a dependency on the simavr library.
class SimavrImpl : public Simavr {
 public:
  static std::unique_ptr<Simavr> Create(
      std::array<uint8_t, 1024> *internal_eeprom_data);

  ~SimavrImpl() override = default;

  void Run() override;

  void InitGdb() override;
  void DeinitGdb() override;
  void Reset() override;
  void Terminate() override;

  int InvokeIoctl(uint32_t ioctl, void *param) override;

  std::unique_ptr<Lifetime> RegisterUsbAttachCallback(
      UsbAttachCallback *callback) override;
  std::unique_ptr<Lifetime> RegisterUartOutputCallback(
      UartOutputCallback *callback) override;
  std::unique_ptr<Lifetime> RegisterI2cMessageCallback(
      I2cMessageCallback *callback) override;

  void RaiseI2cIrq(uint8_t direction, uint32_t value) override;

  void SetData(uint8_t idx, uint8_t val) override;
  void SetState(uint8_t val) override;
  void SetGdbPort(uint16_t val) override;

  uint8_t GetData(uint8_t idx) const override;
  uint8_t GetState() const override;
  uint16_t GetGdbPort() const override;
  uint64_t GetCycle() const override;
  uint32_t GetProgramCounter() const override;
  uint16_t GetStackPointer() const override;
  uint16_t GetBssSectionSize() const override;
  uint16_t GetDataSectionSize() const override;
  uint16_t GetRamSize() const override;
  uint32_t TwiIrqMsg(uint8_t msg, uint8_t addr, uint8_t data) const override;

 protected:
  static std::unique_ptr<avr_t> ParseElfFile(
      elf_firmware_t *firmware,
      std::array<uint8_t, 1024> *internal_eeprom_data);

  SimavrImpl(std::unique_ptr<avr_t> avr,
             std::unique_ptr<elf_firmware_t> firmware);

  std::unique_ptr<avr_t> avr_;
  std::unique_ptr<elf_firmware_t> firmware_;
  avr_irq_t *i2c_irq_;
};
}  // namespace simulator
}  // namespace threeboard