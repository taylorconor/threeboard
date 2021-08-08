#include "flags.h"

#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "third_party/cxxopts.hpp"

namespace threeboard {
namespace simulator {

// Static.
Flags Flags::ParseFromArgs(int argc, char* argv[]) {
  cxxopts::Options options("simulator");
  // clang-format off
  options.add_options()
      ("p,gdb_port", "GDB port", cxxopts::value<uint16_t>()->default_value("1234"))
      ("w,wait_for_debugger", "Wait on program start for GDB to attach", cxxopts::value<bool>()->default_value("false"))
      ("f,shortcut_filename", "Path to shortcut storage file", cxxopts::value<std::string>()->default_value("~/.threeboard_sim"))
      ("h,help", "Print usage")
  ;
  // clang-format on

  try {
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    Flags flags{};
    flags.gdb_port_ = result["gdb_port"].as<uint16_t>();
    flags.gdb_break_start_ = result["wait_for_debugger"].as<bool>();
    flags.shortcut_filename_ = result["shortcut_filename"].as<std::string>();
    if (absl::StrContains(flags.shortcut_filename_, "~")) {
      flags.shortcut_filename_ = absl::StrReplaceAll(flags.shortcut_filename_,
                                                     {{"~", getenv("HOME")}});
    }
    return flags;
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Argument error: " << e.what() << std::endl;
    std::cout << options.help() << std::endl;
    exit(0);
  }
}

}  // namespace simulator
}  // namespace threeboard