#include "pad.h"

#include <curses.h>

#include <mutex>

namespace threeboard {
namespace simulator {

#define S(n, c) std::string(n, c)

Pad::Pad(std::recursive_mutex *write_mutex, int rows, int cols)
    : write_mutex_(write_mutex) {
  pad_ = newpad(rows, cols);
  scrollok(pad_, true);
}

void Pad::Write(const std::string &line) {
  std::lock_guard<std::recursive_mutex> lock(*write_mutex_);
  wprintw(pad_, line.c_str());
}

void Pad::Write(char c) {
  std::lock_guard<std::recursive_mutex> lock(*write_mutex_);
  wprintw(pad_, "%c", c);
}

void Pad::Refresh(const std::string &title, int title_color, int start_row,
                  int start_col, int end_row, int end_col) {
  std::lock_guard<std::recursive_mutex> lock(*write_mutex_);
  move(start_row, start_col);
  auto color = COLOR_PAIR(title_color);
  attron(color);
  printw("- %s %s", title.c_str(),
         S(end_col - title.length() - 2, '-').c_str());
  attroff(color);

  prefresh(pad_, 0, 0, start_row + 1, start_col, end_row, end_col);
}
}  // namespace simulator
}  // namespace threeboard