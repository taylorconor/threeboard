#include "simulator.h"

#include <iostream>

#include "simulator/util/logging.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

// Used to test a single pin in a register.
bool IsEnabled(uint8_t reg, uint8_t pin) { return reg & (1 << pin); }

}  // namespace

Simulator::Simulator(Flags *flags, Simavr *simavr, StateStorage *state_storage)
    : flags_(flags),
      simavr_(simavr),
      is_running_(false),
      firmware_(simavr_),
      usb_host_(simavr_, this),
      uart_(simavr_, this),
      eeprom0_(simavr_, state_storage, I2cEeprom::Instance::EEPROM_0) {
  char log_file[L_tmpnam];
  if (std::tmpnam(log_file)) {
    log_file_path_ = std::string(log_file);
    log_stream_.open(log_file_path_, std::ios_base::app);
  } else {
    std::cout << "Failed to create log file" << std::endl;
    exit(0);
  }
}

Simulator::~Simulator() {
  is_running_ = false;
  sim_run_var_.notify_all();
}

void Simulator::Run() {
  ui_ = std::make_unique<UI>(this, &firmware_, log_file_path_);
  Logging::Init(ui_.get(), &log_stream_);
  ui_->StartAsyncRenderLoop();

  is_running_ = true;
  // Check if we need to enable GDB on program start.
  if (flags_->GetGdbBreakStart()) {
    firmware_.EnableGdb(flags_->GetGdbPort());
  }
  firmware_.RunAsync();
  std::unique_lock<std::mutex> lock(mutex_);
  while (is_running_) {
    sim_run_var_.wait(lock);
  }
}

void Simulator::PrepareRenderState() {
  const uint8_t portb = firmware_.GetPortB();
  const uint8_t portc = firmware_.GetPortC();
  const uint8_t portd = firmware_.GetPortD();
  const uint8_t portf = firmware_.GetPortF();

  // Clear the LED state before the next frame is calculated.
  ui_->ClearLedState();

  // The ERR/STATUS LED pair for threeboard v1 is wired as follows:
  // status0: PB6, ERR
  // status1: PC6, STATUS
  if (IsEnabled(portb, 6) && !IsEnabled(portc, 6)) {
    ui_->SetErr(true);
  } else if (IsEnabled(portc, 6) && !IsEnabled(portb, 6)) {
    ui_->SetStatus(true);
  }

  // The wiring of the LED matrix for threeboard v1 is detailed below. Scanning
  // is row-major, meaning only one row is active at a time. The column pins
  // can be effectively considered as active low.
  // row0: PD7, B0_{4,5,6,7}
  // row1: PB4, B0_{0,1,2,3}
  // row2: PD6, B1_{4,5,6,7}
  // row3: PD4, B1_{0,1,2,3}
  // row4: PB5, (R, G, B, PROG)
  // col0: PF0, (R, B0_3, B0_7, B1_3, B1_7)
  // col1: PF1, (G, B0_2, B0_6, B1_2, B1_6)
  // col2: PF4, (B, B0_1, B0_5, B1_1, B1_5)
  // col3: PF5, (PROG, B0_0, B0_4, B1_0, B1_4)
  if (IsEnabled(portd, 7)) {
    ui_->SetBank0(!IsEnabled(portf, 0), 7);
    ui_->SetBank0(!IsEnabled(portf, 1), 6);
    ui_->SetBank0(!IsEnabled(portf, 4), 5);
    ui_->SetBank0(!IsEnabled(portf, 5), 4);
  } else if (IsEnabled(portb, 4)) {
    ui_->SetBank0(!IsEnabled(portf, 0), 3);
    ui_->SetBank0(!IsEnabled(portf, 1), 2);
    ui_->SetBank0(!IsEnabled(portf, 4), 1);
    ui_->SetBank0(!IsEnabled(portf, 5), 0);
  } else if (IsEnabled(portd, 6)) {
    ui_->SetBank1(!IsEnabled(portf, 0), 7);
    ui_->SetBank1(!IsEnabled(portf, 1), 6);
    ui_->SetBank1(!IsEnabled(portf, 4), 5);
    ui_->SetBank1(!IsEnabled(portf, 5), 4);
  } else if (IsEnabled(portd, 4)) {
    ui_->SetBank1(!IsEnabled(portf, 0), 3);
    ui_->SetBank1(!IsEnabled(portf, 1), 2);
    ui_->SetBank1(!IsEnabled(portf, 4), 1);
    ui_->SetBank1(!IsEnabled(portf, 5), 0);
  } else if (IsEnabled(portb, 5)) {
    ui_->SetR(!IsEnabled(portf, 0));
    ui_->SetG(!IsEnabled(portf, 1));
    ui_->SetB(!IsEnabled(portf, 4));
    ui_->SetProg(!IsEnabled(portf, 5));
  }
}

void Simulator::HandlePhysicalKeypress(char key, bool state) {
  // Simulator command keys.
  if (key == 'q') {
    is_running_ = false;
    sim_run_var_.notify_all();
  } else if (key == 'g') {
    if (firmware_.IsGdbEnabled()) {
      firmware_.DisableGdb();
    } else {
      firmware_.EnableGdb(flags_->GetGdbPort());
    }
  }

  // The key pins are all active low.
  else if (key == 'a') {
    // Switch 1 - maps to PB2.
    firmware_.SetPinB(2, !state);
  } else if (key == 's') {
    // Switch 2 - maps to PB3.
    firmware_.SetPinB(3, !state);
  } else if (key == 'd') {
    // Switch 3 - maps to PB1.
    firmware_.SetPinB(1, !state);
  }
}

void Simulator::HandleVirtualKeypress(uint8_t mod_code, uint8_t key_code) {
  bool capitalise = false;
  // Check for L_SHIFT and R_SHIFT.
  if ((mod_code & 0x22) > 0 && (mod_code & ~0x22) == 0) {
    capitalise = true;
  } else if (mod_code != 0) {
    // Ignore and reject any non-shift modcodes.
    return;
  }

  char c;
  if (key_code >= 0x04 && key_code <= 0x1d) {
    c = key_code + 0x5d;
    if (capitalise) {
      c -= 0x20;
    }
  } else if (key_code == 0x2a) {
    c = ' ';
  } else if (key_code == 0x2d) {
    c = '-';
  } else if (key_code == 0x36) {
    c = ',';
  } else if (key_code == 0x37) {
    c = '.';
  } else {
    // Ignore unsupported characters.
    // TODO: support special characters!
    return;
  }

  ui_->DisplayKeyboardCharacter(c);
}

void Simulator::HandleUartLogLine(const std::string &log_line) {
  auto cycle = firmware_.GetCpuCycleCount();
  log_stream_ << cycle << "\t" << log_line << std::endl;
  ui_->DisplayFirmwareLogLine(cycle, log_line);
}

Flags *Simulator::GetFlags() { return flags_; }

bool Simulator::IsUsbAttached() { return usb_host_.IsAttached(); }

}  // namespace simulator
}  // namespace threeboard
