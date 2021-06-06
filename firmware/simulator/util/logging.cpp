#include "logging.h"

namespace threeboard {
namespace simulator {

void Logging::Init(UI* ui) { ui_ = ui; }

void Logging::Log(const char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), fmt, va);
  va_end(va);
  ui_->DisplaySimulatorLogLine(std::string(buffer));
}

UI* Logging::ui_;

}  // namespace simulator
}  // namespace threeboard