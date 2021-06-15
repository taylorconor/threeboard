#pragma once

#include <fstream>

#include "simulator/ui/ui.h"

#define LOG(fmt, ...)                                          \
  do {                                                         \
    ::threeboard::simulator::Logging::Log(fmt, ##__VA_ARGS__); \
  } while (0)

namespace threeboard {
namespace simulator {

class Logging {
 public:
  static void Init(UI *ui, std::ofstream *log_stream);

  static void Log(const char *fmt, ...);

 private:
  static UI *ui_;
  static std::ofstream *log_stream_;
};

}  // namespace simulator
}  // namespace threeboard