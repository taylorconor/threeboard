load(
  "//toolchain:avr.bzl",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "keypress_handler_delegate",
  hdrs = ["keypress_handler_delegate.h"],
)

cc_library(
  name = "keypress_handler_delegate_mock",
  hdrs = ["keypress_handler_delegate_mock.h"],
)

avr_library(
  name = "usb_interrupt_handler_delegate",
  hdrs = ["usb_interrupt_handler_delegate.h"],
)

avr_library(
  name = "timer_interrupt_handler_delegate",
  hdrs = ["timer_interrupt_handler_delegate.h"],
)
