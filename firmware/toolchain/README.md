# AVR Bazel build rules

This directory contains some special Bazel build rules to produce joint x86 and AVR
build targets. This allows defining a single target definition per BUILD file to
produce libraries for both targets, to avoid having to double-declare them.

`avr.bzl` contains a number of rule definitions to work with AVR libraries and
binaries:
- `avr_library`: similar to `cc_library`, produces a regular `cc_library` target
along with an AVR target.
- `avr_pure_library`: similar to `avr_library` except does not produce a
`cc_library` target.
- `avr_binary`: similar to `cc_binary`, produces an AVR binary file.
- `avr_hex`: produces a hex file to be used for flashing AVR microcontrollers.

Any `cc_library` or `cc_test` targeting non-AVR targets can depend on
`avr_library` targets and will automatically depend on the non-AVR version of that
target. This is because `avr_library` is a rule that produces two targets. One
regular `cc_library` target, and one `avr_pure_library` target, with a name ending
in `_avr`.