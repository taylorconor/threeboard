package(default_visibility = [
    "//integration:__subpackages__",
    "//simulator:__subpackages__",
])

cc_library(
    name = "instrumenting_simavr",
    testonly = 1,
    srcs = ["instrumenting_simavr.cpp"],
    hdrs = ["instrumenting_simavr.h"],
    deps = [
        "//simulator/simavr:simavr_impl",
        "//util:status_util",
        "@abseil//absl/container:flat_hash_map",
        "@abseil//absl/status",
        "@abseil//absl/strings",
    ],
)

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
        "@third_party//:simavr",
    ],
)
