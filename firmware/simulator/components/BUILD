load("@avr-bazel//:avr.bzl", "avr_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_binary(
    name = "threeboard_sim_binary",
    srcs = ["//src:main.c"],
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
    name = "usb_host",
    srcs = ["usb_host.cpp"],
    hdrs = ["usb_host.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr:simavr",
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
    ],
)

cc_library(
    name = "firmware_state_delegate",
    hdrs = ["firmware_state_delegate.h"],
)

cc_library(
    name = "firmware_lib",
    srcs = ["firmware.cpp"],
    hdrs = ["firmware.h"],
    data = [":threeboard_sim_binary"],
    deps = [
        ":firmware_state_delegate",
        "//simulator/simavr:simavr",
    ],
)

cc_library(
    name = "uart",
    srcs = ["uart.cpp"],
    hdrs = ["uart.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr:simavr",
    ],
)
