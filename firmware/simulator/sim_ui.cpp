#include "sim_ui.h"

#include <curses.h>
#include <iostream>

namespace threeboard {
namespace simulator {
namespace {
#define S(n, c) std::string(n, c)

constexpr uint8_t kKeyHoldTime = 20;
constexpr uint8_t kRootX = 1;
constexpr uint8_t kRootY = 1;
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

std::string ParseCpuState(int state) {
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

SimUI::SimUI(const StateUpdateCallback &state_update_callback,
             const KeypressCallback &keypress_callback, const int &sim_state,
             const uint64_t &sim_cycle)
    : state_update_callback_(state_update_callback),
      keypress_callback_(keypress_callback), sim_state_(sim_state),
      sim_cycle_(sim_cycle) {}

SimUI::~SimUI() {
  if (is_running_) {
    is_running_ = false;
    render_thread_->join();
    endwin();
  }
}

void SimUI::StartRenderLoopAsync() {
  if (render_thread_) {
    std::cout << "Attempted to start another SimUI render thread without "
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
  render_thread_ = std::make_unique<std::thread>(&SimUI::RenderLoop, this);
}

void SimUI::ClearLedState() {
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

void SimUI::SetR(bool enabled) { SetLedState(r_, enabled); }
void SimUI::SetG(bool enabled) { SetLedState(g_, enabled); }
void SimUI::SetB(bool enabled) { SetLedState(b_, enabled); }
void SimUI::SetProg(bool enabled) { SetLedState(prog_, enabled); }
void SimUI::SetErr(bool enabled) { SetLedState(err_, enabled); }
void SimUI::SetStatus(bool enabled) { SetLedState(status_, enabled); }
void SimUI::SetBank0(bool enabled, uint8_t idx) {
  SetLedState(bank0_[idx], enabled);
}
void SimUI::SetBank1(bool enabled, uint8_t idx) {
  SetLedState(bank1_[idx], enabled);
}

// Update the internal state of the keys, and trigger any relevant keypress
// callbacks.
void SimUI::UpdateKeyState() {
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
      keypress_callback_(Key::KEY_A, true);
    }
    if (c == 's') {
      key_s_ = kKeyHoldTime;
      keypress_callback_(Key::KEY_S, true);
    }
    if (c == 'd') {
      key_d_ = kKeyHoldTime;
      keypress_callback_(Key::KEY_D, true);
    }
    // A hack to bind the 'Q' key to "quit".
    if (c == 'q') {
      keypress_callback_(Key::KEY_Q, true);
    }
  }

  // Trigger any necessary keyup callbacks.
  if (key_a_ == 0 && keyup_a) {
    keypress_callback_(Key::KEY_A, false);
  }
  if (key_s_ == 0 && keyup_s) {
    keypress_callback_(Key::KEY_S, false);
  }
  if (key_d_ == 0 && keyup_d) {
    keypress_callback_(Key::KEY_D, false);
  }
}

void SimUI::RenderLoop() {
  while (is_running_) {
    state_update_callback_();
    UpdateKeyState();
    DrawBorder();
    DrawLeds();
    DrawKeys();
    DrawStatusText();
    refresh();
    // 200 FPS.
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 / kSimulatorFps));
  }
}

// Draw the LEDs based on the state of the simulator as we know it.
void SimUI::DrawLeds() {
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

void SimUI::DrawKeys() {
  DrawKey(2, key_a_, 'A');
  DrawKey(12, key_s_, 'S');
  DrawKey(22, key_d_, 'D');
}

void SimUI::DrawStatusText() {
  move(kRootY + 1, kRootX + 47);
  printw("threeboard v1");
  move(kRootY + 2, kRootX + 47);
  printw("freq: %s", GetClockSpeed().c_str());
  move(kRootY + 3, kRootX + 47);
  printw("state: %s", ParseCpuState(sim_state_).c_str());
  move(kRootY + 4, kRootX + 47);
  printw("usb state: DISCONNECTED");
}

std::string SimUI::GetClockSpeed() {
  if (cycles_since_print_++ < kSimulatorFps) {
    return cycle_str_;
  }

  cycles_since_print_ = 0;
  uint64_t current_cycle = sim_cycle_;
  uint64_t diff = current_cycle - prev_sim_cycle_;
  if (prev_sim_cycle_ > current_cycle) {
    diff = current_cycle + ((uint64_t)-1 - prev_sim_cycle_);
  }
  prev_sim_cycle_ = sim_cycle_;
  cycle_str_ = ParseCpuFreq(diff);
  return cycle_str_;
}
} // namespace simulator
} // namespace threeboard
