load("@avr-bazel//:avr.bzl", "avr_binary", "avr_hex", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

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
    name = "event_buffer",
    srcs = ["event_buffer.cpp"],
    hdrs = ["event_buffer.h"],
    deps = [
        "//src/delegates:keypress_handler_delegate",
        "//src/native",
    ],
)

avr_library(
    name = "logging",
    srcs = ["logging.cpp"],
    hdrs = ["logging.h"],
    deps = ["//src/native"],
)

avr_library(
    name = "threeboard_lib",
    srcs = ["threeboard.cpp"],
    hdrs = ["threeboard.h"],
    deps = [
        ":event_buffer",
        ":key_controller",
        ":led_controller",
        "//src/native",
        "//src/usb",
    ],
)

avr_library(
    name = "bootstrap",
    srcs = ["bootstrap.cpp"],
    hdrs = ["bootstrap.h"],
    deps = [
        ":logging",
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
