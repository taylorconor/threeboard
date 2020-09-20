load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "constants",
    hdrs = ["constants.h"],
)

avr_library(
    name = "mcu",
    hdrs = ["mcu.h"],
)

avr_library(
    name = "native",
    hdrs = ["native.h"],
    deps = [
        ":constants",
        "//src/delegates:timer_interrupt_handler_delegate",
        "//src/delegates:usb_interrupt_handler_delegate",
    ],
)

cc_library(
    name = "native_mock",
    hdrs = ["native_mock.h"],
    deps = [
        ":native",
        "@gtest",
    ],
)

avr_pure_library(
    name = "native_impl",
    srcs = ["native_impl.cpp"],
    hdrs = ["native_impl.h"],
    deps = [
        ":mcu",
        ":native",
        "//src/delegates:timer_interrupt_handler_delegate",
    ],
)

avr_pure_library(
    name = "i2c",
    srcs = ["i2c.cpp"],
    hdrs = ["i2c.h"],
    deps = [":native"],
)
