#pragma once

#include "simavr.h"

#include <memory>

#include "simavr/sim_avr_types.h"

namespace threeboard {
namespace simulator {

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
  void RegisterUsbAttachCallback(UsbAttachCallback *callback) override;
  void RegisterUartOutputCallback(UartOutputCallback *callback) override;

  void SetData(uint8_t idx, uint8_t val) override;
  void SetState(uint8_t val) override;
  void SetGdbPort(uint8_t val) override;

  uint8_t GetData(uint8_t idx) override;
  uint8_t GetState() override;
  uint8_t GetGdbPort() override;
  uint64_t GetCycle() override;

private:
  SimavrImpl(std::unique_ptr<avr_t> avr);

  std::unique_ptr<avr_t> avr_;
};
} // namespace simulator
} // namespace threeboard