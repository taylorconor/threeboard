load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "i2c_eeprom",
    srcs = ["i2c_eeprom.cpp"],
    hdrs = ["i2c_eeprom.h"],
    deps = [
        "//src/native:native",
        "//src/native:mcu",
        "//src/util:util",
    ],
)