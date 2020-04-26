#include <iostream>

#include "simulator.h"

using namespace threeboard::simulator;

int main() {
  Simulator sim;
  sim.RunAsync();
  std::this_thread::sleep_for(std::chrono::seconds(100));
}
