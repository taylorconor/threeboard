package(default_visibility = ["//visibility:public"])

load(
    "@avr-bazel//:avr.bzl",
    "avr_binary",
)

avr_binary(
    name = "threeboard_sim_binary",
    srcs = ["//src:main.c"],
    copts = select({
        "@bazel_tools//src/conditions:darwin": ["-DDARWIN"],
        "//conditions:default": ["-DLINUX"],
    }) + [
        "-DTHREEBOARD_SIM",
        "-g",
    ],
    deps = [
        "//src:bootstrap",
        "@third_party//:simavr_avr_hdrs",
    ],
)

cc_library(
    name = "simulator_delegate",
    hdrs = ["simulator_delegate.h"],
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
        "//simulator/core:sim_32u4",
        "@third_party//:simavr",
    ],
)

cc_library(
    name = "simulator_lib",
    srcs = ["simulator.cpp"],
    hdrs = ["simulator.h"],
    deps = [
        ":firmware_lib",
        ":simulator_delegate",
        "//simulator/ui",
        "//simulator/usb:host",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = [
        ":simulator_lib",
    ],
)
