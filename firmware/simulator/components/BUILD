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
        "//simulator/simavr",
        "//simulator/ui:ui_delegate",
    ],
)

cc_library(
    name = "usb_host",
    hdrs = ["usb_host.h"],
)

cc_library(
    name = "usb_host_impl",
    srcs = ["usb_host_impl.cpp"],
    hdrs = ["usb_host_impl.h"],
    deps = [
        ":usb_host",
        "//simulator:simulator_delegate",
        "//simulator/simavr",
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
    ],
)

cc_library(
    name = "usb_keycodes",
    srcs = ["usb_keycodes.cpp"],
    hdrs = ["usb_keycodes.h"],
)
