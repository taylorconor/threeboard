#include <curses.h>
#include <iostream>

#include "runner.h"

using namespace threeboard::simulator;

int main(int argc, char *argv[]) {
  Runner runner;
  runner.RunSimulator();
}
