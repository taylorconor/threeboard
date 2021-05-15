#include "simavr_impl.h"

#include <iostream>

#include "simavr/avr_ioport.h"
#include "simavr/avr_twi.h"
#include "simavr/avr_uart.h"
#include "simavr/avr_usb.h"
#include "simavr/sim_elf.h"
#include "simavr/sim_gdb.h"
#include "simulator/lifetime.h"

namespace threeboard {
namespace simulator {
namespace {

// C-style trampoline function to bounce the avr_irq_register_notify callback to
// an instance of the provided callback type defined in T, which is assumed to
// have been cast to void* and provided in param.
template <typename T>
void CallbackTrampoline(avr_irq_t *irq, uint32_t value, void *param) {
  auto *callback = (T *)param;
  (*callback)(value);
}

static const char *_ee_irq_names[2] = {
    [TWI_IRQ_INPUT] = "8>eeprom.out",
    [TWI_IRQ_OUTPUT] = "32<eeprom.in",
};
}  // namespace

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
  auto *raw_ptr = new SimavrImpl(std::move(avr_ptr), f.bsssize, f.datasize);
  return std::unique_ptr<SimavrImpl>(raw_ptr);
}

SimavrImpl::SimavrImpl(std::unique_ptr<avr_t> avr, uint16_t bss_size,
                       uint16_t data_size)
    : avr_(std::move(avr)),
      bss_size_(bss_size),
      data_size_(data_size),
      i2c_irq_(nullptr) {}

void SimavrImpl::Run() { avr_run(avr_.get()); }

void SimavrImpl::InitGdb() { avr_gdb_init(avr_.get()); }

void SimavrImpl::DeinitGdb() { avr_deinit_gdb(avr_.get()); }

void SimavrImpl::Reset() { avr_reset(avr_.get()); }

void SimavrImpl::Terminate() { avr_terminate(avr_.get()); }

int SimavrImpl::InvokeIoctl(uint32_t ioctl, void *param) {
  return avr_ioctl(avr_.get(), ioctl, param);
}

std::unique_ptr<Lifetime> SimavrImpl::RegisterUsbAttachCallback(
    UsbAttachCallback *callback) {
  auto *irq = avr_io_getirq(avr_.get(), AVR_IOCTL_USB_GETIRQ(), USB_IRQ_ATTACH);
  avr_irq_register_notify(irq, &CallbackTrampoline<UsbAttachCallback>,
                          (void *)callback);
  return std::make_unique<Lifetime>([irq, callback]() {
    avr_irq_unregister_notify(irq, &CallbackTrampoline<UsbAttachCallback>,
                              (void *)callback);
  });
}

std::unique_ptr<Lifetime> SimavrImpl::RegisterUartOutputCallback(
    UartOutputCallback *callback) {
  auto *irq =
      avr_io_getirq(avr_.get(), AVR_IOCTL_UART_GETIRQ('1'), UART_IRQ_OUTPUT);
  avr_irq_register_notify(irq, &CallbackTrampoline<UartOutputCallback>,
                          (void *)callback);
  return std::make_unique<Lifetime>([irq, callback]() {
    avr_irq_unregister_notify(irq, &CallbackTrampoline<UartOutputCallback>,
                              (void *)callback);
  });
}

std::unique_ptr<Lifetime> SimavrImpl::RegisterI2cMessageCallback(
    I2cMessageCallback *callback) {
  i2c_irq_ = avr_alloc_irq(&avr_->irq_pool, 0, 2, _ee_irq_names);
  avr_irq_register_notify(i2c_irq_ + TWI_IRQ_OUTPUT,
                          &CallbackTrampoline<I2cMessageCallback>,
                          (void *)callback);

  avr_connect_irq(
      i2c_irq_ + TWI_IRQ_INPUT,
      avr_io_getirq(avr_.get(), AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_INPUT));
  avr_connect_irq(
      avr_io_getirq(avr_.get(), AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_OUTPUT),
      i2c_irq_ + TWI_IRQ_OUTPUT);

  return std::make_unique<Lifetime>([this, callback]() {
    avr_irq_unregister_notify(i2c_irq_, &CallbackTrampoline<I2cMessageCallback>,
                              (void *)callback);
    avr_unconnect_irq(
        i2c_irq_ + TWI_IRQ_INPUT,
        avr_io_getirq(avr_.get(), AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_INPUT));
    avr_unconnect_irq(
        avr_io_getirq(avr_.get(), AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_OUTPUT),
        i2c_irq_ + TWI_IRQ_OUTPUT);
  });
}

void SimavrImpl::RaiseI2cIrq(uint8_t direction, uint32_t value) {
  avr_raise_irq(i2c_irq_ + direction, value);
}

void SimavrImpl::SetData(uint8_t idx, uint8_t val) { avr_->data[idx] = val; }

void SimavrImpl::SetState(uint8_t val) { avr_->state = val; }

uint8_t SimavrImpl::GetGdbPort() const { return avr_->gdb_port; }

void SimavrImpl::SetGdbPort(uint8_t val) { avr_->gdb_port = val; }

uint8_t SimavrImpl::GetData(uint8_t idx) const { return avr_->data[idx]; };

uint8_t SimavrImpl::GetState() const { return avr_->state; }

uint64_t SimavrImpl::GetCycle() const { return avr_->cycle; }

uint16_t SimavrImpl::GetStackPointer() const {
  return avr_->data[R_SPL] | (avr_->data[R_SPH] << 8);
}

uint16_t SimavrImpl::GetBssSectionSize() const { return bss_size_; }

uint16_t SimavrImpl::GetDataSectionSize() const { return data_size_; }

uint16_t SimavrImpl::GetRamSize() const { return avr_->ramend; }

uint32_t SimavrImpl::TwiIrqMsg(uint8_t msg, uint8_t addr, uint8_t data) const {
  return avr_twi_irq_msg(msg, addr, data);
}

}  // namespace simulator
}  // namespace threeboard