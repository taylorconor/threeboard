load(
  "//toolchain:avr.bzl",
  "avr_pure_library",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "usb",
  hdrs = ["usb.h"],
)

avr_library(
  name = "usb_interrupt_handler_delegate",
  hdrs = ["usb_interrupt_handler_delegate.h"],
)

avr_pure_library(
  name = "usb_impl",
  srcs = ["usb_impl.cpp"],
  hdrs = ["usb_impl.h"],
  deps = [
    ":usb",
    "//src/native:native",
    "//src/usb/internal:constants",
    "//src/usb/internal:handlers",
    "//src/usb/internal:descriptors",
  ],
)
