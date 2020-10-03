#include "simavr_impl.h"

#include <iostream>

#include "simavr/avr_ioport.h"
#include "simavr/avr_uart.h"
#include "simavr/avr_usb.h"
#include "simavr/sim_elf.h"
#include "simavr/sim_gdb.h"

namespace threeboard {
namespace simulator {
namespace {

// C-style trampoline function to bounce the avr_irq_register_notify callback to
// the provided callback type defined in T, which is assumed to have been cast
// to void* and provided in param.
template <typename T>
void CallbackTrampoline(avr_irq_t *irq, uint32_t value, void *param) {
  auto *callback = (T *)param;
  (*callback)(value);
}
} // namespace

// static.
std::unique_ptr<Simavr> SimavrImpl::Create(const std::string &firmware_file) {
  elf_firmware_t f;
  if (elf_read_firmware(firmware_file.c_str(), &f)) {
    std::cout << "Failed to read ELF firmware '" << firmware_file << "'"
              << std::endl;
    exit(1);
  }
  std::cout << "Loaded firmware '" << firmware_file << "'" << std::endl;

  avr_t *avr = avr_make_mcu_by_name(f.mmcu);
  if (!avr) {
    std::cout << "Unknown MMCU: '" << f.mmcu << "'" << std::endl;
    exit(1);
  }

  avr_init(avr);
  avr_load_firmware(avr, &f);
  auto avr_ptr = std::unique_ptr<avr_t>(avr);
  auto *raw_ptr = new SimavrImpl(std::move(avr_ptr));
  return std::unique_ptr<SimavrImpl>(raw_ptr);
}

SimavrImpl::SimavrImpl(std::unique_ptr<avr_t> avr) : avr_(std::move(avr)) {}

void SimavrImpl::Run() { avr_run(avr_.get()); }

void SimavrImpl::InitGdb() { avr_gdb_init(avr_.get()); }

void SimavrImpl::DeinitGdb() { avr_deinit_gdb(avr_.get()); }

void SimavrImpl::Reset() { avr_reset(avr_.get()); }

void SimavrImpl::Terminate() { avr_terminate(avr_.get()); }

int SimavrImpl::InvokeIoctl(uint32_t ioctl, void *param) {
  return avr_ioctl(avr_.get(), ioctl, param);
}

void SimavrImpl::RegisterUsbAttachCallback(UsbAttachCallback *callback) {
  avr_irq_register_notify(
      avr_io_getirq(avr_.get(), AVR_IOCTL_USB_GETIRQ(), USB_IRQ_ATTACH),
      &CallbackTrampoline<UsbAttachCallback>, (void *)callback);
}

void SimavrImpl::RegisterUartOutputCallback(UartOutputCallback *callback) {
  avr_irq_register_notify(
      avr_io_getirq(avr_.get(), AVR_IOCTL_UART_GETIRQ('1'), UART_IRQ_OUTPUT),
      &CallbackTrampoline<UartOutputCallback>, (void *)callback);
}

void SimavrImpl::SetData(uint8_t idx, uint8_t val) { avr_->data[idx] = val; }

void SimavrImpl::SetState(uint8_t val) { avr_->state = val; }

uint8_t SimavrImpl::GetGdbPort() { return avr_->gdb_port; }

void SimavrImpl::SetGdbPort(uint8_t val) { avr_->gdb_port = val; }

uint8_t SimavrImpl::GetData(uint8_t idx) { return avr_->data[idx]; };

uint8_t SimavrImpl::GetState() { return avr_->state; }

uint64_t SimavrImpl::GetCycle() { return avr_->cycle; }

} // namespace simulator
} // namespace threeboard