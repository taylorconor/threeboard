load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flags",
    srcs = ["flags.cpp"],
    hdrs = ["flags.h"],
    deps = [
        "//third_party:cxxopts",
        "@abseil//absl/strings",
    ],
)

cc_library(
    name = "lifetime",
    hdrs = ["lifetime.h"],
)

cc_library(
    name = "logging",
    srcs = ["logging.cpp"],
    hdrs = ["logging.h"],
    deps = ["//simulator/ui"],
)

cc_library(
    name = "state_storage",
    srcs = ["state_storage.cpp"],
    hdrs = ["state_storage.h"],
    deps = [
        "//util:status_util",
        "@abseil//absl/status",
        "@nlohmann//:json",
    ],
)
