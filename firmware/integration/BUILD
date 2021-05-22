load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "simulated_test_base",
    hdrs = ["simulated_test_base.h"],
    deps = [
        "@gtest",
        "@gtest//:gtest_main",
        "//simulator/simavr:simavr_impl",
    ],
    alwayslink = 1,
    testonly = 1,
)

cc_test(
    name = "integration_test",
    srcs = ["integration_test.cpp"],
    deps = [":simulated_test_base"],
)