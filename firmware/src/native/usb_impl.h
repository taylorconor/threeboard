#pragma once

#include "usb.h"

namespace threeboard {
namespace native {
class UsbImpl : public Usb {
public:
  void Setup() override;
  void SendKeypress(const uint8_t key, const uint8_t mod) override;
};
} // namespace native
} // namespace threeboard
