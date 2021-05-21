package(default_visibility = ["//simulator:__subpackages__", "//integration:__subpackages__"])

cc_library(
    name = "simavr",
    hdrs = ["simavr.h"],
    deps = [
        "//simulator/simavr/internal:sim_32u4",
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
