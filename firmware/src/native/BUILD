load(
  "//toolchain:avr.bzl",
  "avr_pure_library",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "native",
  hdrs = ["native.h"],
)

avr_pure_library(
  name = "native_impl",
  srcs = ["native_impl.cpp"],
  hdrs = ["native_impl.h"],
  deps = [":native"],
)

cc_library(
  name = "native_mock",
  hdrs = ["native_mock.h"],
  deps = [
    "@gtest//:gtest",
    ":native",
  ],
)

avr_library(
  name = "usb",
  hdrs = ["usb.h"],
)

avr_library(
  name = "usb_common",
  hdrs = ["usb_common.h"],
)

avr_pure_library(
  name = "usb_protocol",
  hdrs = ["usb_protocol.h"],
  srcs = ["usb_protocol.cpp"],
)

avr_pure_library(
  name = "usb_descriptors",
  hdrs = ["usb_descriptors.h"],
  deps = [":usb_protocol"],
)

avr_pure_library(
  name = "usb_handlers",
  srcs = ["usb_handlers.cpp"],
  hdrs = ["usb_handlers.h"],
  deps = [
    ":usb_protocol",
    ":usb_descriptors",
    ":usb_common",
    "//src:led_controller",
    "//src/util:common",
  ],
)

avr_pure_library(
  name = "usb_impl",
  srcs = ["usb_impl.cpp"],
  hdrs = ["usb_impl.h"],
  deps = [
    ":usb",
    ":usb_common",
    ":usb_handlers",
    ":usb_descriptors",
  ],
)
