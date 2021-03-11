load("@avr-bazel//:avr.bzl", "avr_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "error_handler_delegate",
    hdrs = ["error_handler_delegate.h"],
)

cc_library(
    name = "error_handler_delegate_mock",
    hdrs = ["error_handler_delegate_mock.h"],
    deps = [
        "@gtest",
    ],
)

avr_library(
    name = "event_handler_delegate",
    hdrs = ["event_handler_delegate.h"],
)

cc_library(
    name = "event_handler_delegate_mock",
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

avr_library(
    name = "usb_interrupt_handler_delegate",
    hdrs = ["usb_interrupt_handler_delegate.h"],
)

avr_library(
    name = "timer_interrupt_handler_delegate",
    hdrs = ["timer_interrupt_handler_delegate.h"],
)
