package(default_visibility = ["//visibility:public"])

load(
    "@avr-bazel//:avr.bzl",
    "avr_binary",
    "avr_hex",
    "avr_library",
    "avr_pure_library",
)

avr_library(
    name = "led_controller",
    srcs = ["led_controller.cpp"],
    hdrs = ["led_controller.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/native",
        "//src/util:bitset",
    ],
)

cc_test(
    name = "led_controller_test",
    srcs = ["led_controller_test.cpp"],
    deps = [
        ":led_controller",
        "//src/native:native_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

avr_library(
    name = "key_controller",
    srcs = ["key_controller.cpp"],
    hdrs = ["key_controller.h"],
    deps = [
        "//src/delegates:keypress_handler_delegate",
        "//src/native",
        "//src/util",
    ],
)

cc_test(
    name = "key_controller_test",
    srcs = ["key_controller_test.cpp"],
    deps = [
        ":key_controller",
        "//src/delegates:keypress_handler_delegate_mock",
        "//src/native:native_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

avr_library(
    name = "event_handler",
    srcs = ["event_handler.cpp"],
    hdrs = ["event_handler.h"],
    deps = [
        "//src/delegates:keypress_handler_delegate",
        "//src/native",
    ],
)

cc_test(
    name = "event_handler_test",
    srcs = ["event_handler_test.cpp"],
    deps = [
        ":event_handler",
        "//src/native:native_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

avr_library(
    name = "threeboard_lib",
    srcs = ["threeboard.cpp"],
    hdrs = ["threeboard.h"],
    deps = [
        ":event_handler",
        ":key_controller",
        ":led_controller",
        "//src/native",
        "//src/usb",
    ],
)

avr_pure_library(
    name = "bootstrap",
    srcs = ["bootstrap.cpp"],
    hdrs = ["bootstrap.h"],
    deps = [
        ":threeboard_lib",
	"//src/native:i2c",
        "//src/native:native_impl",
        "//src/usb:usb_impl",
    ],
)

avr_binary(
    name = "threeboard_bin",
    srcs = ["main.c"],
    deps = [
      ":bootstrap",
      "//src/native:mcu",
    ],
)

avr_hex(
    name = "threeboard",
    src = ":threeboard_bin",
)
