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
    name = "simulator_lib",
    srcs = ["simulator.cpp"],
    hdrs = ["simulator.h"],
    data = [":threeboard_sim_binary"],
    deps = [
        "//simulator/core:sim_32u4",
        "@third_party//:simavr",
    ],
)

cc_library(
    name = "runner",
    srcs = ["runner.cpp"],
    hdrs = ["runner.h"],
    deps = [
        ":simulator_delegate",
        ":simulator_lib",
        "//simulator/ui",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = [
        ":runner",
    ],
)
