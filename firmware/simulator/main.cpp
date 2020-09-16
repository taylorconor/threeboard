#include <curses.h>
#include <iostream>

#include "simulator.h"

using namespace threeboard::simulator;

int main(int argc, char *argv[]) {
  Simulator runner;
  runner.Run();
}
