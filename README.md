# [threeboard](https://threeboard.dev) [![Build Status](https://travis-ci.com/taylorconor/threeboard.svg?token=2Am1JHeqFB3E9FdETW4w&branch=master)](https://travis-ci.com/taylorconor/threeboard)

<!-- TODO: update this image to an animation when it's ready. -->
<img src="documentation/images/top.png" align="right"/>

threeboard is a fully-functional mechanical USB keyboard with only three keys! It's not a macro pad; it can do just as much as the keyboard you currently have on your desk. It's also programmable and supports multiple layers. It's built completely from scratch, no Arduino involved.

Unlike a traditional keyboard, characters don't show up on the screen after each keypress, because there aren't enough keys. Instead, combinations of the three keys are used to specify key and modifier codes on two built-in 8-bit LED binary indicators. This can then be sent to the host computer as USB keycodes. The multiple layers allow programming and storing macros into the threeboards storage for later retrieval.

## Why?

The main goal of this project is to help people learn about embedded software development. threeboard is a self-contained hardware and firmware project built from scratch, with no external dependencies. This means that all components, from the USB stack to the PCB hardware designs, are written from the ground and are all contained in this repository.

This repository also contains extensive documentation, both within the firmware and in a set of markdown documentation. The primary design goal of the C++ firmware code is readability. It's extensively tested, including end-to-end integration tests which run the complete firmware in a hardware simulator to emulate the physical hardware.

## How to try it out

<!-- TODO: update this image to an animation when it's ready. -->
<img src="documentation/images/helloworld.gif" align="right" width="400" />

The threeboard project includes a terminal-based graphical threeboard simulator, built on top of the [simavr](https://github.com/buserror/simavr) AVR simulator, which runs same firmware file that gets flashed to physical hardware. The simulator also allows remote GDB debugging, so you can step through the threeboard firmware as it's being simulated to learn about it or investigate bugs! See [`src/simulator`](taylorconor.com) for documentation about how to build and run the simulator.

Building your own physical threeboard requires soldering tools and components, as well as a physical threeboard PCB. If you're interested in building one, check out the [threeboard build instructions](taylorconor.com).

## Full documentation
<!-- TODO: add links to relevant documentation. -->
### Background reading
[How do USB keyboards work?](taylorconor.com)

### Firmware
[Firmware design overview](taylorconor.com)  
[Using the threeboard simulator](taylorconor.com)  
[Simulated test framework](taylorconor.com)  

### Hardware
[Hardware design overview](taylorconor.com)  
[Manufacturing and assembling the threeboard hardware](taylorconor.com)  
[Component list](taylorconor.com)
