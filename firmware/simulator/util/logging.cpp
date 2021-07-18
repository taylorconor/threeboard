#include "logging.h"

#include <stdarg.h>

namespace threeboard {
namespace simulator {

void Logging::Init(UI* ui, std::ofstream* log_stream) {
  ui_ = ui;
  log_stream_ = log_stream;
}

void Logging::Log(const char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), fmt, va);
  va_end(va);
  std::string str_buffer = std::string(buffer);
  *log_stream_ << str_buffer;
  ui_->DisplaySimulatorLogLine(str_buffer);
}

UI* Logging::ui_;
std::ofstream* Logging::log_stream_;

}  // namespace simulator
}  // namespace threeboard