load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "simulator_delegate",
    hdrs = ["simulator_delegate.h"],
)

cc_library(
    name = "simulator_lib",
    srcs = ["simulator.cpp"],
    hdrs = ["simulator.h"],
    deps = [
        ":simulator_delegate",
        "//simulator/components:firmware_lib",
        "//simulator/components:usb_host",
        "//simulator/simavr",
        "//simulator/ui",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = [
        ":simulator_lib",
        "//simulator/simavr:simavr_impl",
    ],
)
