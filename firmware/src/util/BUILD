load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "bitset",
    hdrs = ["bitset.h"],
)

cc_test(
    name = "bitset_test",
    srcs = ["bitset_test.cpp"],
    deps = [
        ":bitset",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

avr_library(
    name = "util",
    hdrs = ["util.h"],
)
