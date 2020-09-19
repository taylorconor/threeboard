package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ui",
    srcs = ["ui.cpp"],
    hdrs = ["ui.h"],
    linkopts = ["-lcurses"],
    deps = [
        "//simulator:firmware_state_delegate",
        "//simulator:simulator_delegate",
    ],
)
