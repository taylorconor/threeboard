#pragma once

#include <mutex>
#include <string>

typedef struct _win_st WINDOW;

namespace threeboard {
namespace simulator {

// A small, thread-safe abstraction on top of an ncurses pad window to make it
// easier to compose into the simulator UI.
class Pad {
 public:
  Pad(std::recursive_mutex *write_mutex, int rows, int cols);

  void Write(const std::string &line);
  void Write(char c);
  void Refresh(const std::string &title, int title_color, int start_row,
               int start_col, int end_row, int end_col);

 private:
  std::recursive_mutex *write_mutex_;
  WINDOW *pad_;
};
}  // namespace simulator
}  // namespace threeboard
