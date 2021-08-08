load("@avr-bazel//:avr.bzl", "avr_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gtest_util",
    hdrs = ["gtest_util.h"],
    deps = [
        "@abseil//absl/status",
        "@abseil//absl/status:statusor",
        "@gtest",
    ],
)

avr_library(
    name = "status_common",
    hdrs = ["status_common.h"],
)

cc_library(
    name = "status_util",
    hdrs = ["status_util.h"],
    deps = [
        ":status_common",
        "@abseil//absl/status",
        "@abseil//absl/status:statusor",
    ],
)
