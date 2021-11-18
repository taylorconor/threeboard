#pragma once

#include <fstream>

#include "simulator/ui/ui_delegate.h"
#include "src/keypress.h"

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

inline std::ostream &operator<<(std::ostream &os, Keypress &keypress) {
  if (keypress == Keypress::INACTIVE) {
    os << "INACTIVE";
  } else if (keypress == Keypress::Z) {
    os << "Z";
  } else if (keypress == Keypress::Y) {
    os << "Y";
  } else if (keypress == Keypress::X) {
    os << "X";
  } else if (keypress == Keypress::YZ) {
    os << "YZ";
  } else if (keypress == Keypress::XZ) {
    os << "XZ";
  } else if (keypress == Keypress::XY) {
    os << "XY";
  } else if (keypress == Keypress::XYZ) {
    os << "XYZ";
  }
  return os;
}

}  // namespace threeboard