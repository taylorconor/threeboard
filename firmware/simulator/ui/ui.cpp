#include "ui.h"

#include <curses.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace threeboard {
namespace simulator {
namespace {
#define S(n, c) std::string(n, c)

constexpr uint8_t kKeyHoldTime = 20;
constexpr uint8_t kRootX = 1;
constexpr uint8_t kRootY = 0;
constexpr uint8_t kLedPermenance = 50;
constexpr uint8_t kSimulatorFps = 200;

constexpr uint8_t kLightGrey = 1;
constexpr uint8_t kMedGrey = 2;
constexpr uint8_t kDarkGrey = 3;
constexpr uint8_t kBrightRed = 4;

void printwln(const std::string &str) { printw((str + '\n').c_str()); }

void InitialiseColours() {
  start_color();
  init_pair(kLightGrey, COLOR_WHITE, COLOR_BLACK);
  init_color(16, 400, 400, 400);
  init_pair(kMedGrey, 16, COLOR_BLACK);
  init_color(17, 100, 100, 100);
  init_pair(kDarkGrey, 17, COLOR_BLACK);
  init_color(18, 1000, 0, 0);
  init_pair(kBrightRed, 18, COLOR_BLACK);
}

// Draw the base border of the threeboard and erase the previous frame.
void DrawBorder() {
  // Base border.
  move(kRootY, kRootX);
  attron(COLOR_PAIR(kLightGrey));
  printwln("  " + S(41, '_'));
  printwln("  /" + S(41, ' ') + '\\');
  for (int i = 0; i < 12; i++) {
    printwln(" |" + S(43, ' ') + "|");
  }
  printwln("  \\" + S(41, '_') + '/');

  // Draw the labels for the LEDs.
  move(kRootY + 3, kRootX + 3);
  printw(
      ('R' + S(4, ' ') + 'G' + S(4, ' ') + 'B' + S(13, ' ') + "PROG ERR STATUS")
          .c_str());
  for (int i = 0; i < 2; i++) {
    move(kRootY + 5 + (i * 2), kRootX + 3);
    for (int j = 7; j >= 0; j--) {
      printw(((char)('0' + j) + S(4, ' ')).c_str());
    }
  }
  attroff(COLOR_PAIR(kLightGrey));
}

// Draw a single LED in the provided state under the current cursor;
void DrawLed(uint8_t &enabled, const std::string &extra) {
  int colour = enabled ? COLOR_PAIR(kBrightRed) : COLOR_PAIR(kDarkGrey);
  attron(colour);
  printw(("●" + extra).c_str());
  attroff(colour);
  if (enabled < kLedPermenance && enabled > 0) {
    enabled--;
  }
}

void DrawKey(int x_offset, bool pressed, char letter) {
  int colour = pressed ? COLOR_PAIR(kMedGrey) : COLOR_PAIR(kLightGrey);
  attron(colour);
  if (pressed) {
    move(kRootY + 9, kRootX + x_offset + 2);
    printw(S(4, '_').c_str());
    move(kRootY + 10, kRootX + x_offset);
    printw("|\\____/|");
    move(kRootY + 11, kRootX + x_offset);
    printw("||%c   ||", letter);
    move(kRootY + 12, kRootX + x_offset);
    printw("||    ||");
    move(kRootY + 13, kRootX + x_offset);
    printw("||____||");
  } else {
    move(kRootY + 9, kRootX + x_offset + 1);
    printw(S(6, '_').c_str());
    move(kRootY + 10, kRootX + x_offset);
    printw("||%c   ||", letter);
    move(kRootY + 11, kRootX + x_offset);
    printw("||    ||");
    move(kRootY + 12, kRootX + x_offset);
    printw("||____||");
    move(kRootY + 13, kRootX + x_offset);
    printw("|/____\\|");
  }
  attroff(colour);
}

void SetLedState(uint8_t &led, bool enabled) {
  if (enabled) {
    led = kLedPermenance;
  }
}

void ClearLed(uint8_t &led) {
  if (led > 0) {
    led--;
  }
}

std::string ParseCpuFreq(uint64_t ticks_per_sec) {
  if (ticks_per_sec < 1000) {
    return std::to_string(ticks_per_sec) + " Hz";
  }
  if (ticks_per_sec < 1000000) {
    return std::to_string(ticks_per_sec) + " KHz";
  }
  std::string str = std::to_string(ticks_per_sec);
  return str.substr(0, 2) + "." + str.substr(2, 2) + " MHz";
}

std::string GetCpuStateName(int state) {
  switch (state) {
  case 0:
    return "LIMBO";
  case 1:
    return "STOPPED";
  case 2:
    return "RUNNING";
  case 3:
    return "SLEEPING";
  case 4:
    return "STEP";
  case 5:
    return "STEP_DONE";
  case 6:
    return "DONE";
  case 7:
    return "CRASHED";
  default:
    return "UNKNOWN";
  }
}
} // namespace

UI::UI(SimulatorDelegate *sim_delegate, const int &sim_state,
       const uint64_t &sim_cycle, const bool &gdb_enabled)
    : sim_delegate_(sim_delegate), sim_state_(sim_state), sim_cycle_(sim_cycle),
      gdb_enabled_(gdb_enabled) {}

UI::~UI() {
  if (is_running_) {
    is_running_ = false;
    render_thread_->join();
    endwin();
  }
}

void UI::StartRenderLoopAsync() {
  if (render_thread_) {
    std::cout
        << "Attempted to start another simulator UI render thread without "
           "stopping the previous one!"
        << std::endl;
    exit(0);
  }
  setlocale(LC_ALL, "en_US.UTF-8");
  initscr();
  InitialiseColours();
  noecho();
  timeout(0);
  curs_set(0);
  is_running_ = true;
  render_thread_ = std::make_unique<std::thread>(&UI::RenderLoop, this);
}

void UI::ClearLedState() {
  ClearLed(r_);
  ClearLed(g_);
  ClearLed(b_);
  ClearLed(prog_);
  ClearLed(err_);
  ClearLed(status_);
  for (int i = 0; i < 8; i++) {
    ClearLed(bank0_[i]);
    ClearLed(bank1_[i]);
  }
}

void UI::SetR(bool enabled) { SetLedState(r_, enabled); }
void UI::SetG(bool enabled) { SetLedState(g_, enabled); }
void UI::SetB(bool enabled) { SetLedState(b_, enabled); }
void UI::SetProg(bool enabled) { SetLedState(prog_, enabled); }
void UI::SetErr(bool enabled) { SetLedState(err_, enabled); }
void UI::SetStatus(bool enabled) { SetLedState(status_, enabled); }
void UI::SetBank0(bool enabled, uint8_t idx) {
  SetLedState(bank0_[idx], enabled);
}
void UI::SetBank1(bool enabled, uint8_t idx) {
  SetLedState(bank1_[idx], enabled);
}

// Update the internal state of the keys, and trigger any relevant keypress
// callbacks.
void UI::UpdateKeyState() {
  // Decrement existing keypresses until they exhaust their cooldown period.
  bool keyup_a, keyup_s, keyup_d = false;
  if (key_a_ > 0) {
    key_a_--;
    keyup_a = true;
  }
  if (key_s_ > 0) {
    key_s_--;
    keyup_s = true;
  }
  if (key_d_ > 0) {
    key_d_--;
    keyup_d = true;
  }

  // Register keypresses for keys in the current buffer, and trigger keydown
  // callbacks.
  char c;
  while ((c = getch()) > 0) {
    if (c == 'a') {
      key_a_ = kKeyHoldTime;
    } else if (c == 's') {
      key_s_ = kKeyHoldTime;
    } else if (c == 'd') {
      key_d_ = kKeyHoldTime;
    }
    sim_delegate_->HandleKeypress(c, true);
  }

  // Trigger any necessary keyup callbacks.
  if (key_a_ == 0 && keyup_a) {
    sim_delegate_->HandleKeypress('a', false);
  }
  if (key_s_ == 0 && keyup_s) {
    sim_delegate_->HandleKeypress('s', false);
  }
  if (key_d_ == 0 && keyup_d) {
    sim_delegate_->HandleKeypress('d', false);
  }
}

void UI::RenderLoop() {
  while (is_running_) {
    current_frame_++;
    sim_delegate_->PrepareRenderState();
    UpdateKeyState();
    DrawBorder();
    DrawLeds();
    DrawKeys();
    DrawStatusText();
    move(kRootY + 20, kRootX);
    refresh();
    // 200 FPS.
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 / kSimulatorFps));
  }
}

// Draw the LEDs based on the state of the simulator as we know it.
void UI::DrawLeds() {
  // Top row: R, G, B, PROG, ERR and STATUS.
  move(kRootY + 2, kRootX + 3);
  DrawLed(r_, S(4, ' '));
  DrawLed(g_, S(4, ' '));
  DrawLed(b_, S(14, ' '));
  DrawLed(prog_, S(4, ' '));
  DrawLed(err_, S(4, ' '));
  DrawLed(status_, "");

  // First 8-bit LED bank.
  move(kRootY + 4, kRootX + 3);
  for (int j = 7; j >= 0; j--) {
    DrawLed(bank0_[j], S(4, ' '));
  }

  // Second 8-bit LED bank.
  move(kRootY + 6, kRootX + 3);
  for (int j = 7; j >= 0; j--) {
    DrawLed(bank1_[j], S(4, ' '));
  }
}

void UI::DrawKeys() {
  DrawKey(2, key_a_, 'A');
  DrawKey(12, key_s_, 'S');
  DrawKey(22, key_d_, 'D');
}

void UI::DrawStatusText() {
  move(kRootY + 1, kRootX + 47);
  printw("threeboard v1 (x86 simulator)");
  move(kRootY + 2, kRootX + 47);
  printw("freq: %s", GetClockSpeedString().c_str());
  move(kRootY + 3, kRootX + 47);
  UpdateCpuStateBreakdownList();
  printw("state:");
  int i = 0;
  for (; i < state_str_memo_.size(); ++i) {
    move(kRootY + 4 + i, kRootX + 47);
    printw("  %s", state_str_memo_[i].c_str());
  }
  move(kRootY + 4 + i, kRootX + 47);
  printw("gdb: %s (port %d)", (gdb_enabled_ ? "enabled" : "disabled"),
         sim_delegate_->GetGdbPort());
  move(kRootY + 5 + i, kRootX + 47);
  printw("usb state: DISCONNECTED");

  if (cycles_since_memo_update_++ == kSimulatorFps) {
    cycles_since_memo_update_ = 0;
  }
}

std::string UI::GetClockSpeedString() {
  if (cycles_since_memo_update_ < kSimulatorFps) {
    return freq_str_memo_;
  }

  uint64_t current_cycle = sim_cycle_;
  uint64_t diff = current_cycle - prev_sim_cycle_;
  if (prev_sim_cycle_ > current_cycle) {
    diff = current_cycle + ((uint64_t)-1 - prev_sim_cycle_);
  }
  prev_sim_cycle_ = sim_cycle_;
  freq_str_memo_ = ParseCpuFreq(diff);
  return freq_str_memo_;
}

void UI::UpdateCpuStateBreakdownList() {
  auto state = sim_state_;
  if (cpu_mode_distribution_.find(state) == cpu_mode_distribution_.end()) {
    cpu_mode_distribution_[state] = 0;
  } else {
    cpu_mode_distribution_[state]++;
  }

  if (cycles_since_memo_update_ < kSimulatorFps) {
    return;
  }

  state_str_memo_.clear();
  for (const auto &[key, value] : cpu_mode_distribution_) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    double mode_ratio = ((double)(value * 100) / current_frame_);
    ss << GetCpuStateName(key) << " (" << mode_ratio << "%)";
    state_str_memo_.push_back(ss.str());
  }
}

} // namespace simulator
} // namespace threeboard
