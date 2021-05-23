#pragma once

#include <cstdint>

namespace threeboard {
namespace simulator {

class Flags {
 public:
  static Flags ParseFromArgs(int argc, char* argv[]);

  uint16_t GetGdbPort() const { return gdb_port_; };
  bool GetGdbBreakStart() const { return gdb_break_start_; };

 private:
  Flags() = default;

  uint16_t gdb_port_;
  bool gdb_break_start_;
};
}  // namespace simulator
}  // namespace threeboard
