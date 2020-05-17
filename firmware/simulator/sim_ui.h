#pragma once

#include <thread>

namespace threeboard {
namespace simulator {

// This class is responsible for rendering the simulator UI to the terminal. It
// has no knowledge of the simulator's existence, it relies on its owner setting
// the relevant simulator fields via the setter methods. A callback can be
// provided (StateUpdateCallback) which is called before each render frame, to
// allow the owner to set the current state of the simulator in the UI.
class SimUI {
public:
  enum class Key {
    KEY_A, // 'X' key on threeboard.
    KEY_S, // 'Y' key on threeboard.
    KEY_D, // 'Z' key on threeboard.
    KEY_Q, // 'Q' for quit.
  };

  using StateUpdateCallback = std::function<void()>;
  using KeypressCallback = std::function<void(Key, bool)>;

  SimUI(const StateUpdateCallback &, const KeypressCallback &, const int &,
        const uint64_t &);
  ~SimUI();

  void StartRenderLoopAsync();
  void ClearLedState();

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
  std::string GetClockSpeed();

  StateUpdateCallback state_update_callback_;
  KeypressCallback keypress_callback_;
  const int &sim_state_;
  uint64_t prev_sim_cycle_;
  const uint64_t &sim_cycle_;
  std::string cycle_str_;
  uint8_t cycles_since_print_;

  uint8_t key_a_ = 0;
  uint8_t key_s_ = 0;
  uint8_t key_d_ = 0;

  std::atomic<bool> is_running_;

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
