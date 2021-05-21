load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flags",
    srcs = ["flags.cpp"],
    hdrs = ["flags.h"],
    deps = ["//simulator/third_party:cxxopts"],
)

cc_library(
    name = "lifetime",
    hdrs = ["lifetime.h"],
)
