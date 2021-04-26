load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//src/storage:__subpackages__"])

avr_library(
    name = "eeprom",
    hdrs = ["eeprom.h"],
)

avr_library(
    name = "i2c_eeprom",
    srcs = ["i2c_eeprom.cpp"],
    hdrs = ["i2c_eeprom.h"],
    deps = [
        ":eeprom",
        "//src/native",
        "//src/native:mcu",
        "//src/util",
    ],
)

avr_library(
    name = "internal_eeprom",
    srcs = ["internal_eeprom.cpp"],
    hdrs = ["internal_eeprom.h"],
    deps = [
        ":eeprom",
        "//src/native",
        "//src/util",
    ],
)
