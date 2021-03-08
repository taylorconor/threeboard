load("@avr-bazel//:avr.bzl", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "error_handler_proxy",
    hdrs = ["error_handler_proxy.h"],
    deps = [
        "//src/delegates:error_handler_delegate",
    ]
)

avr_library(
    name = "util",
    hdrs = ["util.h"],
)
