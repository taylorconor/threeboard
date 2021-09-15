#include "simavr_impl.h"

#include <array>
#include <iostream>

#include "absl/strings/str_replace.h"
#include "simavr/avr_ioport.h"
#include "simavr/avr_twi.h"
#include "simavr/avr_uart.h"
#include "simavr/avr_usb.h"
#include "simavr/sim_gdb.h"
#include "simulator/util/lifetime.h"
#include "simulator/util/logging.h"

namespace threeboard {
namespace simulator {
namespace {

// Firmware file path, relative to threeboard/firmware. Bazel will guarantee
// this is built since it's listed as a dependency.
const std::string kFirmwareFile = "simulator/native/threeboard_sim_binary.elf";

// C-style trampoline functions to bounce the avr_irq_register_notify and
// avr_register_io_write callbacks to an instance of the provided callback type
// defined in T, which is assumed to have been cast to void* and provided in
// param.
template <typename T>
void CallbackTrampoline(avr_irq_t *irq, uint32_t value, void *param) {
  auto *callback = (T *)param;
  (*callback)(value);
}

template <typename T>
void CallbackTrampoline(avr_t *avr, avr_io_addr_t addr, uint8_t value,
                        void *param) {
  auto *callback = (T *)param;
  (*callback)(value);
  avr->data[addr] = value;
}

static const char *_ee_irq_names[] = {"twi.miso", "twi.mosi"};

}  // namespace

// static.
std::unique_ptr<Simavr> SimavrImpl::Create(
    std::array<uint8_t, 1024> *internal_eeprom_data) {
  auto firmware = std::make_unique<elf_firmware_t>();
  auto avr_ptr = ParseElfFile(firmware.get(), internal_eeprom_data);
  auto *raw_ptr = new SimavrImpl(std::move(avr_ptr), std::move(firmware));
  return std::unique_ptr<SimavrImpl>(raw_ptr);
}

// static.
std::unique_ptr<avr_t> SimavrImpl::ParseElfFile(
    elf_firmware_t *firmware, std::array<uint8_t, 1024> *internal_eeprom_data) {
  if (elf_read_firmware(kFirmwareFile.c_str(), firmware)) {
    std::cout << "Failed to read ELF firmware '" << kFirmwareFile << "'"
              << std::endl;
    exit(1);
  }

  // Forcibly set the initial value of the internal EEPROM by setting the
  // .eeprom section in the ELF file.
  if (firmware->eeprom == nullptr && firmware->eesize == 0) {
    firmware->eeprom = internal_eeprom_data->data();
    firmware->eesize = 1024;
  } else {
    std::cout << "Invalid ELF firmware: the threeboard simulator requires that "
                 "the .eeprom section is not set"
              << std::endl;
    exit(1);
  }

  std::cout << "Loaded firmware '" << kFirmwareFile << "'" << std::endl;

  avr_t *avr = avr_make_mcu_by_name(firmware->mmcu);
  if (!avr) {
    std::cout << "Unknown MMCU: '" << firmware->mmcu << "'" << std::endl;
    exit(1);
  }

  avr_init(avr);
  avr_load_firmware(avr, firmware);
  avr_global_logger_set(
      [](struct avr_t *avr, const int level, const char *format, va_list ap) {
        // Ignore trace and debug logs (level 4 and 5 respectively) because
        // they're too noisy to be useful.
        if (level > 3) {
          return;
        }
        std::string str_format(format);
        str_format = absl::StrReplaceAll(str_format, {{"\n", ". "}});
        Logging::Log(SimulatorSource::SIMAVR, str_format.c_str(), ap);
      });
  return std::unique_ptr<avr_t>(avr);
}

SimavrImpl::SimavrImpl(std::unique_ptr<avr_t> avr,
                       std::unique_ptr<elf_firmware_t> firmware)
    : avr_(std::move(avr)), firmware_(std::move(firmware)), i2c_irq_(nullptr) {}

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

void SimavrImpl::RegisterPortBWriteCallback(PortWriteCallback *callback) {
  avr_register_io_write(avr_.get(), PORTB,
                        &CallbackTrampoline<PortWriteCallback>,
                        (void *)callback);
}

void SimavrImpl::RegisterPortDWriteCallback(PortWriteCallback *callback) {
  avr_register_io_write(avr_.get(), PORTD,
                        &CallbackTrampoline<PortWriteCallback>,
                        (void *)callback);
}

void SimavrImpl::RaiseI2cIrq(uint8_t direction, uint32_t value) {
  avr_raise_irq(i2c_irq_ + direction, value);
}

void SimavrImpl::SetData(uint8_t idx, uint8_t val) { avr_->data[idx] = val; }

void SimavrImpl::SetState(uint8_t val) { avr_->state = val; }

uint16_t SimavrImpl::GetGdbPort() const { return avr_->gdb_port; }

void SimavrImpl::SetGdbPort(uint16_t val) { avr_->gdb_port = val; }

uint8_t SimavrImpl::GetData(uint8_t idx) const { return avr_->data[idx]; };

uint8_t SimavrImpl::GetState() const { return avr_->state; }

uint64_t SimavrImpl::GetCycle() const { return avr_->cycle; }

uint32_t SimavrImpl::GetProgramCounter() const { return avr_->pc; }

uint16_t SimavrImpl::GetStackPointer() const {
  return avr_->data[R_SPL] | (avr_->data[R_SPH] << 8);
}

uint16_t SimavrImpl::GetBssSectionSize() const { return firmware_->bsssize; }

uint16_t SimavrImpl::GetDataSectionSize() const { return firmware_->datasize; }

uint16_t SimavrImpl::GetRamSize() const { return avr_->ramend; }

uint32_t SimavrImpl::TwiIrqMsg(uint8_t msg, uint8_t addr, uint8_t data) const {
  return avr_twi_irq_msg(msg, addr, data);
}

}  // namespace simulator
}  // namespace threeboard