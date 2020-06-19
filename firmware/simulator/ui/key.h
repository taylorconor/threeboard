#pragma once

namespace threeboard {
namespace simulator {

// An enum listing all of the key commands that can be sent to the simulator. It
// exists in this file to avoid a circular dependency between the simulator
// delegate and the ui.
enum class Key {
  KEY_A, // 'X' key on threeboard.
  KEY_S, // 'Y' key on threeboard.
  KEY_D, // 'Z' key on threeboard.
  KEY_Q, // 'Q' for quit.
  KEY_G, // 'G' to toggle debug enable.
};

} // namespace simulator
} // namespace threeboard
