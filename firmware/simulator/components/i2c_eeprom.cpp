#include "i2c_eeprom.h"

namespace threeboard {
namespace simulator {

I2cEeprom::I2cEeprom(Simavr *simavr, uint32_t size_bytes, uint8_t address)
    : simavr_(simavr), size_bytes_(size_bytes), address_(address) {
  buffer_.reserve(size_bytes_);

    //p->irq = avr_alloc_irq(&avr->irq_pool, 0, 2, _ee_irq_names);
    //avr_irq_register_notify(p->irq + TWI_IRQ_OUTPUT, i2c_eeprom_in_hook, p);

    //p->size = size > sizeof(p->ee) ? sizeof(p->ee) : size;
    //if (data)
//        memcpy(p->ee, data, p->size);

    avr_connect_irq(
            p->irq + TWI_IRQ_INPUT,
            avr_io_getirq(avr, i2c_irq_base, TWI_IRQ_INPUT));
    avr_connect_irq(
            avr_io_getirq(avr, i2c_irq_base, TWI_IRQ_OUTPUT),
            p->irq + TWI_IRQ_OUTPUT );
}
} // namespace simulator
} // namespace threeboard