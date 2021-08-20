load("@avr-bazel//:avr.bzl", "avr_binary", "avr_hex", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "led_state",
    hdrs = ["led_state.h"],
)

avr_library(
    name = "led_controller",
    srcs = ["led_controller.cpp"],
    hdrs = ["led_controller.h"],
    deps = [
        ":led_state",
        ":logging",
        "//src/native",
    ],
)

cc_library(
    name = "led_controller_mock",
    testonly = 1,
    hdrs = ["led_controller_mock.h"],
    deps = [
        ":led_controller",
        "@gtest",
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
        "//src:logging",
        "//src/delegates:event_handler_delegate",
        "//src/native",
        "//src/util",
    ],
)

cc_library(
    name = "key_controller_mock",
    testonly = 1,
    hdrs = ["key_controller_mock.h"],
    deps = [
        ":key_controller",
        "@gtest",
    ],
)

cc_test(
    name = "key_controller_test",
    srcs = ["key_controller_test.cpp"],
    deps = [
        ":key_controller",
        "//src/delegates:event_handler_delegate_mock",
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
        "//src/delegates:event_handler_delegate",
    ],
)

avr_library(
    name = "logging",
    srcs = ["logging.cpp"],
    hdrs = ["logging.h"],
    deps = [
        "//src/native",
        "//src/util",
    ],
)

cc_library(
    name = "logging_fake",
    testonly = 1,
    hdrs = ["logging_fake.h"],
    deps = [
        ":logging",
        "//src/native:native_mock",
    ],
)

cc_test(
    name = "logging_test",
    srcs = ["logging_test.cpp"],
    deps = [
        ":logging",
        "//src/native:native_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

avr_library(
    name = "threeboard",
    srcs = ["threeboard.cpp"],
    hdrs = ["threeboard.h"],
    deps = [
        ":event_buffer",
        ":key_controller",
        ":led_controller",
        "//src/delegates:error_handler_delegate",
        "//src/layers:layer_controller",
        "//src/native",
        "//src/storage:storage_controller",
        "//src/usb:usb_controller",
    ],
)

cc_test(
    name = "threeboard_test",
    srcs = ["threeboard_test.cpp"],
    deps = [
        ":key_controller_mock",
        ":led_controller_mock",
        ":logging_fake",
        ":threeboard",
        "//src:event_buffer",
        "//src/layers:layer_controller_mock",
        "//src/native:native_mock",
        "//src/usb:usb_controller_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

avr_pure_library(
    name = "bootstrap",
    srcs = ["bootstrap.cpp"],
    hdrs = ["bootstrap.h"],
    deps = [
        ":logging",
        ":threeboard",
        "//src/native:native_impl",
        "//src/usb:usb_controller_impl",
        "//src/util:error_handler_proxy",
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
    name = "threeboard_hex",
    src = ":threeboard_bin",
)
