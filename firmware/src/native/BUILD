load(
  "//toolchain:avr.bzl",
  "avr_pure_library",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "constants",
  hdrs = ["constants.h"],
)

avr_library(
  name = "timer_interrupt_handler_delegate",
  hdrs = ["timer_interrupt_handler_delegate.h"],
)

avr_library(
  name = "native",
  hdrs = ["native.h"],
  deps = [
    ":constants",
    ":timer_interrupt_handler_delegate",
    "//src/usb:usb_interrupt_handler_delegate",
  ],
)

avr_pure_library(
  name = "native_impl",
  srcs = ["native_impl.cpp"],
  hdrs = ["native_impl.h"],
  deps = [
    ":native",
    ":timer_interrupt_handler_delegate",
  ],
)

cc_library(
  name = "native_mock",
  hdrs = ["native_mock.h"],
  deps = [
    "@gtest//:gtest",
    ":native",
  ],
)

avr_pure_library(
  name = "i2c",
  srcs = ["i2c.cpp"],
  hdrs = ["i2c.h"],
  deps = [":native"],
)
