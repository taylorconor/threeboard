load(
  "//toolchain:avr.bzl",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "usb",
  hdrs = ["usb.h"],
)

avr_library(
  name = "usb_impl",
  srcs = ["usb_impl.cpp"],
  hdrs = ["usb_impl.h"],
  deps = [
    ":usb",
    "//src/delegates:usb_interrupt_handler_delegate",
    "//src/native:native",
    "//src/usb/internal:constants",
    "//src/usb/internal:request_handler",
    "//src/usb/internal:descriptors",
  ],
)

cc_test(
  name = "usb_impl_test",
  linkstatic = 1,
  srcs = ["usb_impl_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    ":usb_impl",
    "//src/native:native_mock",
    "//src/usb/internal:request_handler_mock",
    "//src/usb/testutil:testutil",
  ],
)

cc_test(
  name = "usb_integration_test",
  srcs = ["usb_integration_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    ":usb_impl",
    "//src/usb/testutil:fake_host",
    "//src/delegates:usb_interrupt_handler_delegate",
    "//src/native:native_mock",
  ],
)
