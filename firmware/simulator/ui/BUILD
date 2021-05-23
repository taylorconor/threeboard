load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//simulator:__subpackages__"])

cc_library(
    name = "pad",
    srcs = ["pad.cpp"],
    hdrs = ["pad.h"],
    linkopts = [
        "-lcurses",
    ],
)

cc_library(
    name = "ui",
    srcs = ["ui.cpp"],
    hdrs = ["ui.h"],
    linkopts = [
        "-lcurses",
    ],
    deps = [
        ":pad",
        "//simulator:firmware_state_delegate",
        "//simulator:simulator_delegate",
    ],
)
