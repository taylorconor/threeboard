#include "usb_descriptors.h"

#include <avr/pgmspace.h>

namespace threeboard {
namespace native {
DescriptorContainer DescriptorContainer::ParseFromProgmem(const uint8_t *ptr) {
  DescriptorContainer descriptor;
  descriptor.id = pgm_read_word(ptr);
  descriptor.index = pgm_read_word(ptr + 2);
  descriptor.data = (const uint8_t *)pgm_read_word(ptr + 4);
  descriptor.length = pgm_read_byte(ptr + 6);
  return descriptor;
}
} // namespace native
} // namespace threeboard
