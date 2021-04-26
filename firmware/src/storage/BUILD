load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "storage_controller",
    hdrs = ["storage_controller.h"],
    srcs = ["storage_controller.cpp"],
    deps = [
        "//src/storage/internal:eeprom",
    ],
)