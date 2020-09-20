load("@avr-bazel//:avr.bzl", "avr_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "constants",
    hdrs = ["constants.h"],
)

avr_library(
    name = "protocol",
    srcs = ["protocol.cpp"],
    hdrs = ["protocol.h"],
    deps = [
        "//src/native",
        "//src/util",
    ],
)
