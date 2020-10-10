#include "pad.h"

#include <curses.h>

namespace threeboard {
namespace simulator {

Pad::Pad(int rows, int cols) {
  pad_ = newpad(rows, cols);
  scrollok(pad_, TRUE);
}

void Pad::Write(const std::string &line) { wprintw(pad_, line.c_str()); }

void Pad::Write(char c) { wprintw(pad_, "%c", c); }

void Pad::Refresh(int start_row, int start_col, int end_row, int end_col) {
  prefresh(pad_, 0, 0, start_row, start_col, end_row, end_col);
}
} // namespace simulator
} // namespace threeboard