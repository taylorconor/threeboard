#pragma once

#include <fstream>

#include "simulator/ui/ui_delegate.h"

#define LOG(fmt, ...)                                                      \
  do {                                                                     \
    ::threeboard::simulator::Logging::Log(SimulatorSource::SIMULATOR, fmt, \
                                          ##__VA_ARGS__);                  \
  } while (0)

namespace threeboard {
namespace simulator {

class Logging {
 public:
  static void Init(UIDelegate *ui_delegate, std::ofstream *log_stream);

  static void Log(const SimulatorSource &, const char *fmt, ...);
  static void Log(const SimulatorSource &, const char *fmt, va_list va);

 private:
  static UIDelegate *ui_delegate_;
  static std::ofstream *log_stream_;
};

}  // namespace simulator
}  // namespace threeboard