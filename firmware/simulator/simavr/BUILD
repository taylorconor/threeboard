package(default_visibility = ["//simulator:__subpackages__"])

cc_library(
    name = "simavr",
    hdrs = ["simavr.h"],
    deps = [
        "//simulator:lifetime",
        "//simulator/simavr/internal:sim_32u4",
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
    deps = [
        ":simavr",
        "@third_party//:simavr",
    ],
)
