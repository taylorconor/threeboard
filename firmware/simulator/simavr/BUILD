package(default_visibility = [
    "//integration:__subpackages__",
    "//simulator:__subpackages__",
])

cc_library(
    name = "simavr",
    hdrs = ["simavr.h"],
    deps = [
        "//simulator/simavr/internal:sim_32u4",
        "//simulator/simavr/internal:simavr_headers",
        "//simulator/util:lifetime",
    ],
)

cc_library(
    name = "simavr_mock",
    hdrs = ["simavr_mock.h"],
    deps = [
        ":simavr",
        "@gtest",
    ],
)

cc_library(
    name = "simavr_impl",
    srcs = ["simavr_impl.cpp"],
    hdrs = ["simavr_impl.h"],
    data = ["//simulator/native:threeboard_sim_binary"],
    deps = [
        ":simavr",
        "//simulator/util:logging",
        "@abseil//absl/strings",
        "@third_party//:simavr",
    ],
)
