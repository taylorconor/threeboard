#include "ui.h"

#include <curses.h>

#include <iomanip>
#include <iostream>
#include <sstream>

namespace threeboard {
namespace simulator {
namespace {

#define S(n, c) std::string(n, c)

constexpr uint8_t kRootX = 0;
constexpr uint8_t kRootY = 0;
constexpr uint8_t kOutputBoxY = kRootY + 15;
constexpr uint8_t kLogBoxY = kRootY + 19;
constexpr uint8_t kLogStatusColOffset = 47;

constexpr uint8_t kKeyHoldTime = 20;
constexpr uint8_t kLedPermanence = 50;
constexpr uint8_t kSimulatorFps = 200;

constexpr uint8_t kWhite = 1;
constexpr uint8_t kMedGrey = 2;
constexpr uint8_t kDarkGrey = 3;
constexpr uint8_t kBrightRed = 4;

void printwln(const std::string &str) { printw((str + '\n').c_str()); }

void InitialiseColors() {
  start_color();
  init_pair(kWhite, COLOR_WHITE, COLOR_BLACK);
  init_color(16, 500, 500, 500);
  init_pair(kMedGrey, 16, COLOR_BLACK);
  init_color(17, 100, 100, 100);
  init_pair(kDarkGrey, 17, COLOR_BLACK);
  init_color(18, 1000, 0, 0);
  init_pair(kBrightRed, 18, COLOR_BLACK);
}

// Draw the base border of the threeboard and erase the previous frame.
void DrawBorder() {
  // Base border.
  move(kRootY, 0);
  attron(COLOR_PAIR(kWhite));
  printwln(S(kRootX + 2, ' ') + S(41, '_'));
  printwln(S(kRootX + 1, ' ') + "/" + S(41, ' ') + '\\');
  for (int i = 0; i < 12; i++) {
    printwln(S(kRootX, ' ') + "|" + S(43, ' ') + "|");
  }
  printwln(S(kRootX + 1, ' ') + "\\" + S(41, '_') + '/');

  // Draw the labels for the LEDs.
  move(kRootY + 3, kRootX + 3);
  printw(
      ('R' + S(4, ' ') + 'G' + S(4, ' ') + 'B' + S(12, ' ') + "PROG STATUS ERR")
          .c_str());
  for (int i = 0; i < 2; i++) {
    move(kRootY + 5 + (i * 2), kRootX + 3);
    for (int j = 7; j >= 0; j--) {
      printw(((char)('0' + j) + S(4, ' ')).c_str());
    }
  }
  attroff(COLOR_PAIR(kWhite));
}

// Draw a single LED in the provided state under the current cursor;
void DrawLed(uint8_t &enabled, const std::string &extra) {
  int color = enabled ? COLOR_PAIR(kBrightRed) : COLOR_PAIR(kDarkGrey);
  attron(color);
  printw(("●" + extra).c_str());
  attroff(color);
  if (enabled < kLedPermanence && enabled > 0) {
    enabled--;
  }
}

void DrawKey(int x_offset, bool pressed, char letter) {
  int color = pressed ? COLOR_PAIR(kMedGrey) : COLOR_PAIR(kWhite);
  attron(color);
  if (pressed) {
    move(kRootY + 9, x_offset + 2);
    printw(S(4, '_').c_str());
    move(kRootY + 10, x_offset);
    printw("|\\____/|");
    move(kRootY + 11, x_offset);
    printw("||%c   ||", letter);
    move(kRootY + 12, x_offset);
    printw("||    ||");
    move(kRootY + 13, x_offset);
    printw("||____||");
  } else {
    move(kRootY + 9, x_offset + 1);
    printw(S(6, '_').c_str());
    move(kRootY + 10, x_offset);
    printw("||%c   ||", letter);
    move(kRootY + 11, x_offset);
    printw("||    ||");
    move(kRootY + 12, x_offset);
    printw("||____||");
    move(kRootY + 13, x_offset);
    printw("|/____\\|");
  }
  attroff(color);
}

void SetLedState(uint8_t &led, bool enabled) {
  if (enabled) {
    led = kLedPermanence;
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
    return std::to_string(ticks_per_sec / 1000) + " KHz";
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
}  // namespace

UI::UI(SimulatorDelegate *sim_delegate,
       FirmwareStateDelegate *firmware_state_delegate,
       const std::string &log_file)
    : simulator_delegate_(sim_delegate),
      firmware_state_delegate_(firmware_state_delegate),
      window_(nullptr),
      screen_output_mutex_(std::make_unique<std::recursive_mutex>()),
      is_running_(false),
      log_file_(log_file) {}

UI::~UI() {
  if (is_running_) {
    is_running_ = false;
    render_thread_->join();
    endwin();
  }
}  // namespace simulator

void UI::StartAsyncRenderLoop() {
  if (render_thread_) {
    std::cout
        << "Attempted to start another simulator UI render thread without "
           "stopping the previous one!"
        << std::endl;
    exit(0);
  }
  setlocale(LC_ALL, "en_US.UTF-8");

  // Initialise the main curses window.
  window_ = initscr();
  InitialiseColors();
  noecho();
  timeout(0);
  curs_set(0);

  // Initialise the logging sub-window ('pad'), the scrollable area that
  // displays the contents of the logs.
  int max_x, max_y;
  getmaxyx(window_, max_y, max_x);
  output_pad_ = std::make_unique<Pad>(screen_output_mutex_.get(),
                                      kLogBoxY - kOutputBoxY, max_x);
  log_pad_ = std::make_unique<Pad>(screen_output_mutex_.get(),
                                   max_y - kLogBoxY - 1, max_x);

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

void UI::DisplayKeyboardCharacter(char c) { output_pad_->Write(c); }

void UI::DisplayFirmwareLogLine(uint64_t cycle, const std::string &log_line) {
  std::string cycle_str = std::to_string(cycle);
  std::string log =
      cycle_str + S(16 - cycle_str.length(), ' ') + log_line + "\n";
  log_pad_->Write(log);
}

void UI::DisplaySimulatorLogLine(const std::string &log_line) {
  log_pad_->Write("[simulator]     " + log_line + "\n");
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
  bool keyup_a = false, keyup_s = false, keyup_d = false;
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
    simulator_delegate_->HandlePhysicalKeypress(c, true);
  }

  // Trigger any necessary keyup callbacks.
  if (key_a_ == 0 && keyup_a) {
    simulator_delegate_->HandlePhysicalKeypress('a', false);
  }
  if (key_s_ == 0 && keyup_s) {
    simulator_delegate_->HandlePhysicalKeypress('s', false);
  }
  if (key_d_ == 0 && keyup_d) {
    simulator_delegate_->HandlePhysicalKeypress('d', false);
  }
}

void UI::RenderLoop() {
  while (is_running_) {
    {
      std::lock_guard<std::recursive_mutex> lock(*screen_output_mutex_);
      current_frame_++;

      // Trigger any applicable keypresses from the user into the firmware.
      UpdateKeyState();

      // Tell the simulator that we're about to redraw and need the most recent
      // state set from the firmware onto the UI. This is an optimisation to
      // avoid having the firmware update state more than it needs to.
      simulator_delegate_->PrepareRenderState();

      // Draw each UI component.
      DrawBorder();
      DrawLeds();
      DrawKeys();
      DrawStatusText();
      DrawOutputBox();
      DrawLogBox();

      // Move the cursor to a place where stdio output won't overwrite any of
      // the simulator UI.
      // TODO: is there a way of disabling or suppressing non-curses io?
      move(kRootY + 20, kRootX);
      refresh();
    }

    // Maintain kSimulatorFps FPS.
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
  DrawKey(kRootX + 2, key_a_, 'A');
  DrawKey(kRootX + 12, key_s_, 'S');
  DrawKey(kRootX + 22, key_d_, 'D');
}

void UI::DrawStatusText() {
  const int col_offset = kRootX + kLogStatusColOffset;
  int row_offset = 1;

  // Title text.
  move(kRootY + row_offset++, col_offset);
  printw("threeboard v1 (x86 simulator)");

  // Clock frequency monitor.
  move(kRootY + row_offset++, col_offset);
  printw("clock frequency: %s", GetClockSpeedString().c_str());

  // Memory usage monitor and per-section detail.
  move(kRootY + row_offset++, col_offset);
  printw("memory usage: %s", GetSramUsageString().c_str());
  UpdateSramUsageBreakdownList();
  for (const auto &[section_name, section_bytes] : sram_usage_breakdown_memo_) {
    move(kRootY + row_offset++, col_offset);
    printw("  %s: %d bytes", section_name.c_str(), section_bytes);
  }

  // CPU state distribution.
  move(kRootY + row_offset++, col_offset);
  UpdateCpuStateBreakdownList();
  printw("state:");
  if (state_str_memo_.empty()) {
    printw(" Loading...");
  }
  for (const auto &[text, is_active] : state_str_memo_) {
    move(kRootY + row_offset++, col_offset);
    int color = is_active ? COLOR_PAIR(kWhite) : COLOR_PAIR(kMedGrey);
    attron(color);
    printw("  %s", text.c_str());
    attroff(color);
  }

  // gdb status and port display.
  move(kRootY + row_offset++, col_offset);
  printw("gdb: %s",
         firmware_state_delegate_->IsGdbEnabled() ? "enabled" : "disabled");
  if (firmware_state_delegate_->IsGdbEnabled()) {
    printw(" (port %d)", simulator_delegate_->GetFlags()->GetGdbPort());
  }

  // USB attach status monitor.
  move(kRootY + row_offset, col_offset);
  printw("usb: %s",
         (simulator_delegate_->IsUsbAttached() ? "attached" : "detached"));

  if (cycles_since_memo_update_++ == kSimulatorFps) {
    cycles_since_memo_update_ = 0;
  }
}

void UI::DrawOutputBox() {
  int window_max_x, window_max_y;
  getmaxyx(window_, window_max_y, window_max_x);
  output_pad_->Refresh("keyboard output", kMedGrey, kOutputBoxY + 1, 0,
                       kLogBoxY - 1, window_max_x - 1);
}

void UI::DrawLogBox() {
  int window_max_x, window_max_y;
  getmaxyx(window_, window_max_y, window_max_x);
  log_pad_->Refresh("log file: " + log_file_, kMedGrey, kLogBoxY + 1, 0,
                    window_max_y - 1, window_max_x - 1);
}

std::string UI::GetClockSpeedString() {
  if (cycles_since_memo_update_ == kSimulatorFps) {
    uint64_t current_cycle = firmware_state_delegate_->GetCpuCycleCount();
    uint64_t diff = current_cycle - prev_sim_cycle_;
    if (prev_sim_cycle_ > current_cycle) {
      diff = current_cycle + (~0 - prev_sim_cycle_);
    }
    prev_sim_cycle_ = firmware_state_delegate_->GetCpuCycleCount();
    freq_str_memo_ = ParseCpuFreq(diff);
  }
  return freq_str_memo_;
}

std::string UI::GetSramUsageString() {
  if (cycles_since_memo_update_ == kSimulatorFps) {
    sram_str_memo_ =
        std::to_string(firmware_state_delegate_->GetSramUsage()) + "%";
  }
  return sram_str_memo_;
}

void UI::UpdateCpuStateBreakdownList() {
  auto state = firmware_state_delegate_->GetCpuState();
  cpu_states_since_last_flush_.insert(state);
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
    bool is_active = cpu_states_since_last_flush_.find(key) !=
                     cpu_states_since_last_flush_.end();
    state_str_memo_.emplace_back(ss.str(), is_active);
  }
  cpu_states_since_last_flush_.clear();
}

void UI::UpdateSramUsageBreakdownList() {
  if (cycles_since_memo_update_ != kSimulatorFps) {
    return;
  }

  sram_usage_breakdown_memo_.clear();
  if (firmware_state_delegate_->GetDataSectionSize() != 0) {
    sram_usage_breakdown_memo_.emplace_back(
        ".data", firmware_state_delegate_->GetDataSectionSize());
  }
  if (firmware_state_delegate_->GetBssSectionSize() != 0) {
    sram_usage_breakdown_memo_.emplace_back(
        ".bss", firmware_state_delegate_->GetBssSectionSize());
  }

  // Stack size is always included, if it's zero then something is broken.
  sram_usage_breakdown_memo_.emplace_back(
      "stack", firmware_state_delegate_->GetStackSize());
}

}  // namespace simulator
}  // namespace threeboard
