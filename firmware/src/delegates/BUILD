load("@avr-bazel//:avr.bzl", "avr_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "event_handler_delegate",
    hdrs = ["event_handler_delegate.h"],
    deps = ["//src:keypress"],
)

cc_library(
    name = "event_handler_delegate_mock",
    testonly = 1,
    hdrs = ["event_handler_delegate_mock.h"],
    deps = [
        "@gtest",
    ],
)

avr_library(
    name = "layer_controller_delegate",
    hdrs = ["layer_controller_delegate.h"],
    deps = [
        "//src/layers:layer_id",
    ],
)

cc_library(
    name = "layer_controller_delegate_mock",
    testonly = 1,
    hdrs = ["layer_controller_delegate_mock.h"],
    deps = [
        "@gtest",
    ],
)

avr_library(
    name = "usb_interrupt_handler_delegate",
    hdrs = ["usb_interrupt_handler_delegate.h"],
)

avr_library(
    name = "timer_interrupt_handler_delegate",
    hdrs = ["timer_interrupt_handler_delegate.h"],
)
