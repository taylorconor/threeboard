load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "storage_controller",
    srcs = ["storage_controller.cpp"],
    hdrs = ["storage_controller.h"],
    deps = [
        "//src/native",
        "//src/storage/internal:eeprom",
        "//src/storage/internal:i2c_eeprom",
        "//src/storage/internal:internal_eeprom",
        "//src/usb:usb_controller",
    ],
)

cc_library(
    name = "storage_controller_mock",
    hdrs = ["storage_controller_mock.h"],
    deps = [
        ":storage_controller",
        "@gtest",
    ],
)

cc_test(
    name = "storage_controller_test",
    srcs = ["storage_controller_test.cpp"],
    deps = [
        ":storage_controller",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
