load("@avr-bazel//:avr.bzl", "avr_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_binary(
    name = "threeboard_sim_binary",
    srcs = ["//src:main.c"],
    visibility = ["//visibility:private"],
    copts = select({
        "@bazel_tools//src/conditions:darwin": ["-DDARWIN"],
        "//conditions:default": ["-DLINUX"],
    }) + [
        # This flag enables simavr-specific sections to be added to the
        # firmware file for parsing by simavr.
        "-DTHREEBOARD_SIM",
        "-g",
    ],
    deps = [
        "//src:bootstrap",
        "@third_party//:simavr_avr_hdrs",
    ],
)

cc_library(
    name = "firmware_state_delegate",
    hdrs = ["firmware_state_delegate.h"],
)

cc_library(
    name = "firmware",
    srcs = ["firmware.cpp"],
    hdrs = ["firmware.h"],
    data = [":threeboard_sim_binary"],
    deps = [
        ":firmware_state_delegate",
        "//simulator/simavr",
    ],
)

cc_test(
    name = "firmware_test",
    srcs = ["firmware_test.cpp"],
    deps = [
        ":firmware",
        "//simulator/simavr:simavr_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "uart",
    srcs = ["uart.cpp"],
    hdrs = ["uart.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr",
    ],
)

cc_test(
    name = "uart_test",
    srcs = ["uart_test.cpp"],
    deps = [
        ":uart",
        "//simulator:simulator_delegate_mock",
        "//simulator/simavr:simavr_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "usb_host",
    srcs = ["usb_host.cpp"],
    hdrs = ["usb_host.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr",
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
    ],
)

cc_test(
    name = "usb_host_test",
    srcs = ["usb_host_test.cpp"],
    deps = [
        ":usb_host",
        "//simulator:simulator_delegate_mock",
        "//simulator/simavr:simavr_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
