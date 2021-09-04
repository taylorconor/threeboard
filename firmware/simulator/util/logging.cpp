#include "logging.h"

#include <stdarg.h>

namespace threeboard {
namespace simulator {

void Logging::Init(UIDelegate* ui_delegate, std::ofstream* log_stream) {
  ui_delegate_ = ui_delegate;
  log_stream_ = log_stream;
}

void Logging::Log(const SimulatorSource& source, const char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  Log(source, fmt, va);
  va_end(va);
}

void Logging::Log(const SimulatorSource& source, const char* fmt, va_list va) {
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), fmt, va);
  std::string str_buffer = std::string(buffer);
  if (log_stream_) {
    *log_stream_ << str_buffer << std::endl;
  }
  if (ui_delegate_) {
    ui_delegate_->HandleLogLine(str_buffer, source);
  }
}

UIDelegate* Logging::ui_delegate_;
std::ofstream* Logging::log_stream_;

}  // namespace simulator
}  // namespace threeboard