#pragma once

#include "simulator/components/firmware_state_delegate.h"
#include "simulator/simulator_delegate.h"

#include <atomic>
#include <functional>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace threeboard {
namespace simulator {

// This class is responsible for rendering the simulator UI to the terminal. It
// has no knowledge of the simulator's existence, it relies on its owner setting
// the relevant simulator fields via the setter methods. A callback can be
// provided (StateUpdateCallback) which is called before each render frame, to
// allow the owner to set the current state of the simulator in the UI.
class UI {
public:
  UI(SimulatorDelegate *, FirmwareStateDelegate *);
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
  void DrawLeds();
  void DrawKeys();
  void DrawStatusText();
  std::string GetClockSpeedString();
  void UpdateCpuStateBreakdownList();

  SimulatorDelegate *simulator_delegate_;
  FirmwareStateDelegate *firmware_state_delegate_;

  // Keep track of the simulator cycle count from the previous render pass so we
  // can calculate CPU frequency.
  uint64_t prev_sim_cycle_ = 0;

  // Memoize some stats so we don't have to constantly recalculate and so that
  // their values don't update so fast they flicker and become unreadable.
  uint8_t cycles_since_memo_update_ = 0;
  std::string freq_str_memo_ = "Loading...";
  std::vector<std::pair<std::string, bool>> state_str_memo_ = {
      {"Loading...", false}};

  std::unordered_map<uint8_t, uint64_t> cpu_mode_distribution_;
  std::unordered_set<int> cpu_states_since_last_flush_;

  uint8_t key_a_ = 0;
  uint8_t key_s_ = 0;
  uint8_t key_d_ = 0;

  std::atomic<bool> is_running_;
  uint64_t current_frame_ = 0;

  uint8_t r_ = 0;
  uint8_t g_ = 0;
  uint8_t b_ = 0;
  uint8_t prog_ = 0;
  uint8_t err_ = 0;
  uint8_t status_ = 0;
  uint8_t bank0_[8] = {};
  uint8_t bank1_[8] = {};
  std::unique_ptr<std::thread> render_thread_;
};
} // namespace simulator
} // namespace threeboard
