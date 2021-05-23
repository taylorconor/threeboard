load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = [
    "//integration:__subpackages__",
    "//simulator:__subpackages__",
])

cc_library(
    name = "gtest_util",
    hdrs = ["gtest_util.h"],
    deps = [
        "@abseil//absl/status",
        "@abseil//absl/status:statusor",
        "@gtest",
    ],
)

cc_library(
    name = "status_util",
    hdrs = ["status_util.h"],
    deps = [
        "@abseil//absl/status",
        "@abseil//absl/status:statusor",
    ],
)
