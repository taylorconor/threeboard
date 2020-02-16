#include "descriptors.h"

namespace threeboard {
namespace usb {
DescriptorContainer
DescriptorContainer::ParseFromProgmem(native::Native *native,
                                      const uint8_t *ptr) {
  DescriptorContainer descriptor;
  descriptor.id = native->ReadPgmWord(ptr);
  descriptor.index = native->ReadPgmWord(ptr + 2);
  descriptor.data = (const uint8_t *)native->ReadPgmWord(ptr + 4);
  descriptor.length = native->ReadPgmByte(ptr + 6);
  return descriptor;
}
} // namespace usb
} // namespace threeboard
