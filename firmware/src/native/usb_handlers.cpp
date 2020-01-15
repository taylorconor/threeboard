#include "usb_handlers.h"

#include "../util/common.h"
#include "usb_descriptors.h"

#define ENABLE_ERR() ((PORTF |= 1 << PF6) && (PORTF &= ~(1 << PF7)))
#define ENABLE_STATUS() ((PORTF |= 1 << PF7) && (PORTF &= ~(1 << PF6)))

namespace threeboard {
namespace native {
namespace {
#define NUM_DESC_LIST (sizeof(descriptor_list) / sizeof(DescriptorContainer))

// Misc functions to wait for ready and send/receive packets
static __always_inline void AwaitTransmitterReady() {
  while (!(UEINTX & (1 << TXINI)))
    ;
}
static __always_inline void HandshakeTransmitterInterrupt() { UEINTX = ~(1 << TXINI); }
} // namespace

namespace device_handler {
namespace {

// Find the DescriptorContainer from the descriptor_list in PROGMEM that matches
// the DescriptorType of this packet.
bool FindMatchingContainer(const SetupPacket &packet,
                           DescriptorContainer *descriptor) {
  const DescriptorContainer *ptr = descriptor_list;
  // Find the first descriptor with a matching descriptor value.
  uint8_t i = 0;
  for (; i < NUM_DESC_LIST; i++, ptr++) {
    DescriptorValue descriptor_value = pgm_read_word(ptr);
    if (descriptor_value == packet.wValue) {
      break;
    }
  }
  // Now find the descriptor in the descriptor in the list with a matching
  // wIndex.
  for (; i < NUM_DESC_LIST; i++, ptr++) {
    *descriptor = DescriptorContainer::ParseFromProgmem((uint8_t *)ptr);
    if (descriptor->wIndex == packet.wIndex) {
      return true;
    }
  }
  return false;
}
} // namespace

// Called when the host requests the status of the device.
void HandleGetStatus() {
  AwaitTransmitterReady();
  // The response only contains two status bits. Bit 0 = 0 indicates that this
  // device is not self-powered (it relies on bus power), and bit 1 = 0
  // indicates that the device does not support remote wakeup. We send this
  // response directly into the data register.
  UEDATX = 0;
  HandshakeTransmitterInterrupt();
}

// Allows the host to set the USB address of this device.
void HandleSetAddress(const SetupPacket &packet) {
  // TODO: USB spec section 9.4.6 specifies different behaviours here for
  // default and address state. Might need to implement this.
  HandshakeTransmitterInterrupt();
  AwaitTransmitterReady();
  // wValue contains the 7-bit address. The highest-order bit is the request
  // unspecified.
  UDADDR = packet.wValue;
  // Enable the address by setting the highest-order bit (a feature of the
  // microcontroller, not the USB protocol).
  UDADDR |= 1 << ADDEN;
}

// Returns a descriptor as requested by the host, if such a descriptor exists.
void HandleGetDescriptor(const SetupPacket &packet) {
  DescriptorContainer container;
  auto found = FindMatchingContainer(packet, &container);
  if (!found) {
    // Stall if we can't find a matching DescriptorContainer. This is an
    // unrecoverable error.
    UECONX = (1 << STALLRQ) | (1 << EPEN);
    return;
  }

  uint8_t i, n;
  uint8_t len = (packet.wLength < 256) ? packet.wLength : 255;
  if (len > container.length) {
    len = container.length;
  }
  do {
    // wait for host ready for IN packet
    do {
      i = UEINTX;
    } while (!(i & ((1 << TXINI) | (1 << RXOUTI))));
    if (i & (1 << RXOUTI)) {
      return; // abort
    }
    // send IN packet
    n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
    for (i = n; i; i--) {
      UEDATX = pgm_read_byte(container.addr++);
    }
    len -= n;
    HandshakeTransmitterInterrupt();
  } while (len || n == ENDPOINT0_SIZE);
}

// Replies with the current Configuration of the device.
void HandleGetConfiguration(const volatile uint8_t &usb_configuration) {
  // TODO: Section 9.4.2 specifies different configs for different states.
  // Should also implement that here.
  AwaitTransmitterReady();
  UEDATX = usb_configuration;
  HandshakeTransmitterInterrupt();
}

// Allows the host to specify the current Configuration of the device.
void HandleSetConfiguration(const SetupPacket &packet,
                            volatile uint8_t *usb_configuration) {
  *usb_configuration = packet.wValue;
  HandshakeTransmitterInterrupt();
  // TODO: remainder may not be necessary.
  const uint8_t *cfg = endpoint_config_table;
  for (uint8_t i = 1; i < 5; i++) {
    UENUM = i;
    uint8_t en = pgm_read_byte(cfg++);
    UECONX = en;
    if (en) {
      UECFG0X = pgm_read_byte(cfg++);
      UECFG1X = pgm_read_byte(cfg++);
    }
  }
  UERST = 0x1E;
  UERST = 0;
}

} // namespace device_handler
} // namespace native
} // namespace threeboard
