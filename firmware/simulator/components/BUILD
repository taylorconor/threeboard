load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "i2c_eeprom",
    srcs = ["i2c_eeprom.cpp"],
    hdrs = ["i2c_eeprom.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr",
        "//simulator/util:logging",
        "//simulator/util:state_storage",
    ],
)

cc_library(
    name = "uart",
    srcs = ["uart.cpp"],
    hdrs = ["uart.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr",
    ],
)

cc_test(
    name = "uart_test",
    srcs = ["uart_test.cpp"],
    deps = [
        ":uart",
        "//simulator:simulator_delegate_mock",
        "//simulator/simavr:simavr_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "usb_host",
    srcs = ["usb_host.cpp"],
    hdrs = ["usb_host.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/simavr",
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
    ],
)

cc_test(
    name = "usb_host_test",
    srcs = ["usb_host_test.cpp"],
    deps = [
        ":usb_host",
        "//simulator:simulator_delegate_mock",
        "//simulator/simavr:simavr_mock",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
