load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "firmware_state_delegate",
    hdrs = ["firmware_state_delegate.h"],
)

cc_library(
    name = "firmware",
    srcs = ["firmware.cpp"],
    hdrs = ["firmware.h"],
    data = ["//simulator/native:threeboard_sim_binary"],
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
    name = "simulator_delegate",
    hdrs = ["simulator_delegate.h"],
    deps = ["//simulator/util:flags"],
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
        "//simulator:firmware",
        "//simulator/components:i2c_eeprom",
        "//simulator/components:uart",
        "//simulator/components:usb_host",
        "//simulator/simavr",
        "//simulator/ui",
        "//simulator/util:flags",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = [
        ":simulator_lib",
        "//simulator/simavr:simavr_impl",
        "//simulator/util:flags",
    ],
)
