#pragma once

#include <cstdint>
#include <string>

namespace threeboard {
namespace simulator {

// A class to collect and allow access to all the command line flag values
// passed to the simulator at startup.
class Flags {
 public:
  static Flags ParseFromArgs(int argc, char* argv[]);

  uint16_t GetGdbPort() const { return gdb_port_; };
  bool GetGdbBreakStart() const { return gdb_break_start_; };
  std::string GetShortcutFilename() const { return shortcut_filename_; }

 private:
  Flags() = default;

  uint16_t gdb_port_;
  bool gdb_break_start_;
  std::string shortcut_filename_;
};
}  // namespace simulator
}  // namespace threeboard
