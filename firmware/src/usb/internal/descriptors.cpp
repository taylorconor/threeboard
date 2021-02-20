#include "descriptors.h"

namespace threeboard {
namespace usb {
DescriptorContainer DescriptorContainer::ParseFromProgmem(
    native::Native *native, const uint8_t *ptr) {
  DescriptorContainer descriptor;
  descriptor.id = native->ReadPgmWord(ptr);
  ptr += sizeof(descriptor.id);
  descriptor.index = native->ReadPgmWord(ptr);
  ptr += sizeof(descriptor.index);
  descriptor.data = (const uint8_t *)native->ReadPgmWord(ptr);
  ptr += sizeof(descriptor.data);
  descriptor.length = native->ReadPgmByte(ptr);
  return descriptor;
}
}  // namespace usb
}  // namespace threeboard
