load(
    "@avr-bazel//:avr.bzl",
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
        "//src/native",
        "//src/usb/internal:constants",
        "//src/usb/internal:descriptors",
        "//src/usb/internal:request_handler",
    ],
)

cc_test(
    name = "usb_impl_test",
    srcs = ["usb_impl_test.cpp"],
    linkstatic = 1,
    deps = [
        ":usb_impl",
        "//src/native:native_mock",
        "//src/usb/internal:request_handler_mock",
        "//src/usb/testutil",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "usb_integration_test",
    srcs = ["usb_integration_test.cpp"],
    deps = [
        ":usb_impl",
        "//src/delegates:usb_interrupt_handler_delegate",
        "//src/native:native_mock",
        "//src/usb/testutil:fake_host",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
