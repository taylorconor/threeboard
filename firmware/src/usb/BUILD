load("@avr-bazel//:avr.bzl", "avr_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "usb_controller",
    hdrs = ["usb_controller.h"],
)

cc_library(
    name = "usb_controller_mock",
    hdrs = ["usb_controller_mock.h"],
    deps = [
        ":usb_controller",
        "@gtest",
    ],
)

avr_library(
    name = "usb_controller_impl",
    srcs = ["usb_controller_impl.cpp"],
    hdrs = ["usb_controller_impl.h"],
    deps = [
        ":usb_controller",
        "//src:logging",
        "//src/delegates:error_handler_delegate",
        "//src/delegates:usb_interrupt_handler_delegate",
        "//src/native",
        "//src/usb/internal:descriptors",
        "//src/usb/internal:request_handler",
        "//src/usb/shared:constants",
    ],
)

cc_test(
    name = "usb_controller_impl_test",
    srcs = ["usb_controller_impl_test.cpp"],
    linkstatic = 1,
    deps = [
        ":usb_controller_impl",
        "//src:logging_fake",
        "//src/delegates:error_handler_delegate_mock",
        "//src/native:native_mock",
        "//src/usb/internal:request_handler_mock",
        "//src/usb/testutil",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)