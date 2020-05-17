#include <curses.h>
#include <iostream>

#include "sim_runner.h"

using namespace threeboard::simulator;

int main(int argc, char *argv[]) {
  SimRunner runner;
  runner.RunSimulator();
}
