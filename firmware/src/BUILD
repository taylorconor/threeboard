load(
  "//toolchain:avr.bzl",
  "avr_binary",
  "avr_library",
  "avr_hex",
)

avr_library(
  name = "led_controller",
  srcs = ["led_controller.cpp"],
  hdrs = ["led_controller.h"],
  deps = [
    "//src/util:bitset",
    "//src/native:native",
  ],
)

cc_test(
  name = "led_controller_test",
  srcs = ["led_controller_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//src/native:native_mock",
    ":led_controller"
  ],
)

avr_library(
  name = "key_controller",
  srcs = ["key_controller.cpp"],
  hdrs = ["key_controller.h"],
  deps = [
    "//src/native:native",
    "//src/util:bitset",
  ],
)

avr_library(
  name = "event_handler",
  srcs = ["event_handler.cpp"],
  hdrs = ["event_handler.h"],
  deps = [
    "//src/native:native",
    ":key_controller",
  ],
)

avr_library(
  name = "threeboard_lib",
  srcs = ["threeboard.cpp"],
  hdrs = ["threeboard.h"],
  deps = [
    ":led_controller",
    ":key_controller",
    ":event_handler",
    "//src/native:native",
    "//src/util:enums",
  ]
)

avr_binary(
  name = "threeboard_bin",
  srcs = ["main.cpp"],
  deps = [
    ":threeboard_lib",
    "//src/native:native_impl",
  ],
)

avr_hex(
  name = "threeboard",
  src = ":threeboard_bin",
)
