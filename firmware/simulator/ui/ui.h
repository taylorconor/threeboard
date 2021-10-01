#pragma once

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "simulator/simulator.h"
#include "simulator/ui/pad.h"
#include "simulator/ui/ui_delegate.h"
#include "simulator/util/flags.h"

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
class UI : public UIDelegate {
 public:
  UI(Simulator *, Flags *);
  ~UI();

  void Run();

  void HandleLogLine(const std::string &) override;
  void HandleLogLine(const std::string &,
                     const SimulatorSource &source) override;

 private:
  void RenderLoop();

  void UpdateKeyState();

  void UpdateCpuStateBreakdownList();
  void UpdateSramUsageBreakdownList();

  std::string GetClockSpeedString();
  std::string GetSramUsageString();

  void DrawLeds();
  void DrawKeys();
  void DrawStatusText();
  void DrawOutputBox();
  void DrawLogBox();

  Simulator *simulator_;
  Flags *flags_;

  // Current state of the simulator for a given render cycle.
  SimulatorState current_sim_state_;
  DeviceState current_device_state_;

  // The output window used by curses.
  WINDOW *window_;

  std::unique_ptr<Pad> output_pad_;
  std::unique_ptr<Pad> log_pad_;

  // Mutex to avoid text output conflicts so we don't have to depend on the
  // pthread-enabled ncurses library build, which may not be available on all
  // platforms.
  std::unique_ptr<std::recursive_mutex> screen_output_mutex_;

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

  std::atomic<bool> is_running_;
  uint64_t current_frame_ = 0;

  uint8_t key_a_ = 0;
  uint8_t key_s_ = 0;
  uint8_t key_d_ = 0;
};
}  // namespace simulator
}  // namespace threeboard
