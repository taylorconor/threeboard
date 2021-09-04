load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//simulator:__subpackages__"])

cc_library(
    name = "pad",
    srcs = ["pad.cpp"],
    hdrs = ["pad.h"],
    linkopts = select({
        # On macOS there is no -lncurses or -lncursesw, -lcurses is an alias
        # for -lncursesw. So we need to treat it specially here to avoid making
        # building and running on macOS more annoying.
        "@bazel_tools//src/conditions:darwin": ["-lcurses"],
        "//conditions:default": [
            "-lncursesw",
        ],
    }),
)

cc_library(
    name = "ui",
    srcs = ["ui.cpp"],
    hdrs = ["ui.h"],
    linkopts = select({
        "@bazel_tools//src/conditions:darwin": ["-lcurses"],
        "//conditions:default": [
            "-lncursesw",
        ],
    }),
    deps = [
        ":pad",
        "//simulator:simulator_lib",
    ],
)

cc_library(
    name = "ui_delegate",
    hdrs = ["ui_delegate.h"],
)