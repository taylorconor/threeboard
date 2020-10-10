#pragma once

#include <string>

typedef struct _win_st WINDOW;

namespace threeboard {
namespace simulator {

// A small abstraction on top of an ncurses pad window to make it easier to
// compose into the simulator UI.
// TODO: add title output and thread safety to the responsibility of this class.
class Pad {
public:
  Pad(int rows, int cols);

  void Write(const std::string &line);
  void Write(char c);
  void Refresh(int start_row, int start_col, int end_row, int end_col);

private:
  WINDOW *pad_;
};
} // namespace simulator
} // namespace threeboard
