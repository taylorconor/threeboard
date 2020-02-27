load(
  "//toolchain:avr.bzl",
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

avr_library(
  name = "protocol",
  hdrs = ["protocol.h"],
  srcs = ["protocol.cpp"],
  deps = [
    "//src/util:util",
    "//src/native:native",
  ],
)

avr_library(
  name = "descriptors",
  hdrs = ["descriptors.h"],
  srcs = ["descriptors.cpp"],
  deps = [
    ":constants",
    ":protocol",
    "//src/util:util"
  ],
)

avr_library(
  name = "request_handler",
  srcs = ["request_handler.cpp"],
  hdrs = ["request_handler.h"],
  deps = [
    ":protocol",
    ":descriptors",
    ":constants",
    ":hid_state",
    "//src/native:native",
    "//src/util:util",
  ],
)

cc_library(
  name = "request_handler_mock",
  testonly = 1,
  hdrs = ["request_handler_mock.h"],
  deps = [
    ":request_handler",
  ],
)
