# threeboard

<img src="documentation/images/top.png" align="right"/>

threeboard is a fully-functional open source and open hardware mechanical USB computer keyboard with only three keys. It supports multiple programmable layers, and achieves the same functionality as a full-sized keyboard. Its firmware and hardware are built completely from scratch and are extensively documented.

Unlike a traditional keyboard, characters don't show up on the screen after each keypress, because there aren't enough keys. Instead, combinations of the three keys are used to specify key and modifier codes on two built-in 8-bit LED binary indicators. This can then be sent to the host computer as USB keycodes. The multiple layers allow users to program macros into the threeboard’s storage for quick retrieval. All functionality is documented in the [threeboard user manual](documentation/threeboard/threeboard_user_manual.md).

## Motivation
The main goal of the threeboard project is to build a relatively easy to understand, readable, self-contained and well-documented embedded software and hardware project. The threeboard is built from scratch, with no external dependencies. This means that all components, from the USB stack to the PCB hardware designs, are written from the ground up and are all contained in this repository.

The threeboard project is extensively documented: within the firmware itself as comments, in this set of detailed markdown documents, and in a [PDF design document](documentation/threeboard_design_doc.pdf). The primary design goal of the C++ firmware code is readability, to enable as many people to read and learn from it as possible, regardless of background. It's well tested, including end-to-end integration tests which execute tests against the firmware in a hardware simulator to emulate the physical hardware.


## How to try it out

<img src="documentation/images/simulator.gif" align="right" width="400"/>

The threeboard project includes a terminal-based graphical firmware simulator capable of simulating the entire functionality of the threeboard. You can build and run the simulator yourself by reading the [simulator build instructions](documentation/threeboard/simulator_build_instructions.md). The simulator is built on top of the [simavr](https://github.com/buserror/simavr) AVR simulator, and runs the exact same firmware file that gets flashed to physical hardware. The simulator also allows remote debugging with GDB, so you can step through the threeboard firmware as it's being simulated to learn about it or investigate bugs!

Building your own physical threeboard requires a physical threeboard PCB, soldering tools and components, as well as some knowledge of electronics and soldering. If you're interested in building one, check out the [threeboard hardware build instructions](documentation/threeboard/hardware_build_instructions.md).

## Full documentation
Below are markdown files documenting various aspects of the threeboard project in detail. Full 41-page documentation is available in PDF format in the [threeboard design doc](documentation/threeboard_design_doc.pdf) file.

### Background reading 
[threeboard user manual](documentation/threeboard/threeboard_user_manual.md)  
[How do USB keyboards work?](documentation/threeboard/how_usb_keyboards_work.md) 

### Firmware
[Firmware design](documentation/threeboard/firmware_design.md)  
[Firmware build instructions](documentation/threeboard/firmware_build_instructions.md)  
[Simulator user manual & design](documentation/threeboard/simulator_manual_and_design.md)  
[Simulator build instructions](documentation/threeboard/simulator_build_instructions.md)  

### Hardware
[Hardware design](documentation/threeboard/hardware_design.md)  
[Hardware build instructions](documentation/threeboard/hardware_build_instructions.md)  
[Component list](documentation/threeboard/component_list.md)
