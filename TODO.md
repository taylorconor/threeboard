# TODO

## General
### Documentation
- [ ] Add overview documentation to each subdirectory.

### Code style
- [ ] Write an explicit clang-format style file.
- [ ] Enforce code formatting in a presubmit rule.

## Firmware
- [ ] Implement proper state changes in threeboard.cpp.
  - [ ] Implement a proper state interface.

### Storage
- [ ] Add eeprom implementation
- [ ] Add i2c implementation

### USB
- [ ] Make everything strictly typed.
  - [ ] In particular, strict typing for USB State.
- [ ] Standard packet definitions.
  - [ ] Strictly-typed PID types and names.
  - [ ] USB p196.
- [ ] Add a src/native/usb_internal package:
  - [ ] Objects that should not escape src/native.
- [ ] Add tests and mocks for USB code, in particular, usb_handlers.
  - [ ] We can add a usb_native class, or a native_internal class or whatever, which exposes methods like native_->UEDATX().
- [ ] Convert the descriptor_list to use usb_protocol.h structs.
- [ ] Merge usb_protocol and usb_descriptors.

## Simulator
- [ ] Add tests for all simulator classes.
  - [ ] Introduce a shim layer to make simavr testable and injectable.
- [ ] Add eeprom simulation
- [ ] Add i2c simulation