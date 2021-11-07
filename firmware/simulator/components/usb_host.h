#pragma once

#include <iostream>

namespace threeboard {
namespace simulator {

class UsbHost {
 public:
  virtual ~UsbHost() = default;

  virtual bool IsAttached() const = 0;
};
}  // namespace simulator
}  // namespace threeboard
