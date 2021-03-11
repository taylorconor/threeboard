#pragma once

#include "src/delegates/error_handler_delegate.h"

namespace threeboard {
namespace util {

class ErrorHandlerProxy : public ErrorHandlerDelegate {
 public:
  void SetImpl(ErrorHandlerDelegate* impl) { impl_ = impl; }

 private:
  ErrorHandlerDelegate* impl_;
};
}  // namespace util
}  // namespace threeboard