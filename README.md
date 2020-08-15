# [threeboard](https://threeboard.dev) [![Build Status](https://travis-ci.com/taylorconor/threeboard.svg?token=2Am1JHeqFB3E9FdETW4w&branch=master)](https://travis-ci.com/taylorconor/threeboard)

<!-- TODO: update this image to an animation when it's ready. -->
<img src="documentation/images/top.png" align="right"/>

threeboard is a fully functional multi-layer, programmable, mechanical USB keyboard with only three keys! That means it can do just as much as the keyboard you currently have on your desk, using only three keys.

Unlike a traditional keyboard, keypresses aren't sent directly to the computer on each keypress, because there aren't enough keys. Instead, combinations of the three keys are used to specify key and modifier codes which can then be sent to the host computer as USB keycodes. The multiple layers allow committing key, word, and text blob shortcuts to storage for later retrieval.

## Why?

The main goal of this project is to act as a pedagogical example of a self-contained hardware and firmware project built from scratch, with no external dependencies. This means that all components, from the USB stack to the PCB hardware designs, are written from scratch and contained in this repository.

It also contains extensive documentation, both within the firmware and in a set of markdown documents. The C++ firmware code intentionally puts readability over performance, although it remains very concise and performant. It's extensively tested, including end-to-end integration tests which run the complete firmware in a hardware simulator to emulate the physical hardware.

## How to try it out

If you'd like to build your own threeboard, check out the [threeboard build instructions](taylorconor.com).

<!-- TODO: update this image to an animation when it's ready. -->
<img src="documentation/images/bottom.png" align="right"/>

If you don't want to build a physical threeboard, you can still try it out using the simulator. The threeboard project includes a terminal-based graphical threeboard simulator, built on top of the [simavr](https://github.com/buserror/simavr) AVR simulator, which runs same firmware file that gets flashed to physical hardware. Only macOS and Linux are supported by simavr. The simulator also allows remote GDB debugging, so you can step through the threeboard firmware as it's being simulated to learn about it or investigate bugs! See [`src/simulator`](taylorconor.com) for documentation about how to build and run the simulator.

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
