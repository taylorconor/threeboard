load(
  "//toolchain:avr.bzl",
  "avr_pure_library",
  "avr_library",
)

package(default_visibility = ["//src/usb:__subpackages__"])

avr_library(
  name = "constants",
  hdrs = ["constants.h"],
)

avr_library(
  name = "hid_state",
  hdrs = ["hid_state.h"],
)

avr_pure_library(
  name = "protocol",
  hdrs = ["protocol.h"],
  srcs = ["protocol.cpp"],
  deps = [
    "//src/native:native",
  ],
)

avr_pure_library(
  name = "descriptors",
  hdrs = ["descriptors.h"],
  srcs = ["descriptors.cpp"],
  deps = [
    ":constants",
    ":protocol",
    "//src/util:util"
  ],
)

avr_pure_library(
  name = "handlers",
  srcs = ["handlers.cpp"],
  hdrs = ["handlers.h"],
  deps = [
    ":protocol",
    ":descriptors",
    ":constants",
    ":hid_state",
    "//src/util:util",
  ],
)
