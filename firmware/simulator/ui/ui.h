#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "simulator/components/firmware_state_delegate.h"
#include "simulator/simulator_delegate.h"

// Forward declaration of ncurses WINDOW. Defined in curses.h and included by
// ui.cpp.
typedef struct _win_st WINDOW;

namespace threeboard {
namespace simulator {

// This class is responsible for rendering the simulator UI to the terminal.
// It has no knowledge of the simulator's existence, it relies on its owner
// setting the relevant simulator fields via the setter methods. A callback
// can be provided (StateUpdateCallback) which is called before each render
// frame, to allow the owner to set the current state of the simulator in
// the UI.
class UI {
public:
  UI(SimulatorDelegate *, FirmwareStateDelegate *, const std::string &log_file);
  ~UI();

  void StartAsyncRenderLoop();
  void ClearLedState();

  void DisplayKeyboardCharacter(char);
  void DisplayLogLine(uint64_t cycle, const std::string &);

  void SetR(bool);
  void SetG(bool);
  void SetB(bool);
  void SetProg(bool);
  void SetErr(bool);
  void SetStatus(bool);
  void SetBank0(bool, uint8_t);
  void SetBank1(bool, uint8_t);

private:
  void UpdateKeyState();
  void RenderLoop();
  std::string GetClockSpeedString();
  std::string GetSramUsageString();

  void UpdateCpuStateBreakdownList();
  void UpdateSramUsageBreakdownList();

  void DrawLeds();
  void DrawKeys();
  void DrawStatusText();
  void DrawLogBox();

  SimulatorDelegate *simulator_delegate_;
  FirmwareStateDelegate *firmware_state_delegate_;

  // The output window used by curses.
  WINDOW *window_;
  // The pad (a special case of a window) used to display the log file.
  WINDOW *log_pad_;

  // Mutex to avoid text output conflicts so we don't have to depend on the
  // pthread-enabled ncurses library build, which may not be available on all
  // platforms.
  std::mutex output_mutex_;

  // Keep track of the simulator cycle count from the previous render pass so we
  // can calculate CPU frequency.
  uint64_t prev_sim_cycle_ = 0;

  // Memoize some stats so we don't have to constantly recalculate and so that
  // their values don't update so fast they flicker and become unreadable.
  uint8_t cycles_since_memo_update_ = 0;
  std::string freq_str_memo_ = "Loading...";
  std::string sram_str_memo_ = "Loading...";
  std::vector<std::pair<std::string, bool>> state_str_memo_;
  std::vector<std::pair<std::string, uint16_t>> sram_usage_breakdown_memo_;

  std::unordered_map<uint8_t, uint64_t> cpu_mode_distribution_;
  std::unordered_set<int> cpu_states_since_last_flush_;

  std::unique_ptr<std::thread> render_thread_;
  std::atomic<bool> is_running_;
  std::string log_file_;
  uint64_t current_frame_ = 0;

  uint8_t key_a_ = 0;
  uint8_t key_s_ = 0;
  uint8_t key_d_ = 0;
  uint8_t r_ = 0;
  uint8_t g_ = 0;
  uint8_t b_ = 0;
  uint8_t prog_ = 0;
  uint8_t err_ = 0;
  uint8_t status_ = 0;
  uint8_t bank0_[8] = {};
  uint8_t bank1_[8] = {};
};
} // namespace simulator
} // namespace threeboard
