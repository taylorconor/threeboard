load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "lifetime",
    hdrs = ["lifetime.h"],
)

cc_library(
    name = "simulator_delegate",
    hdrs = ["simulator_delegate.h"],
)

cc_library(
    name = "simulator_delegate_mock",
    hdrs = ["simulator_delegate_mock.h"],
    deps = [
        ":simulator_delegate",
        "@gtest",
    ],
)

cc_library(
    name = "simulator_lib",
    srcs = ["simulator.cpp"],
    hdrs = ["simulator.h"],
    deps = [
        ":simulator_delegate",
        "//simulator/components:firmware",
        "//simulator/components:i2c_eeprom",
        "//simulator/components:uart",
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
