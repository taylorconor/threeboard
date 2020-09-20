load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ui",
    srcs = ["ui.cpp"],
    hdrs = ["ui.h"],
    linkopts = ["-lcurses"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/components:firmware_state_delegate",
    ],
)
