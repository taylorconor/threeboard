load("@rules_cc//cc:defs.bzl", "cc_library")

cc_test(
    name = "integration_test",
    srcs = ["integration_test.cpp"],
    deps = [
        ":simulated_test_base",
        "//util:gtest_util",
    ],
)

cc_library(
    name = "simulated_test_base",
    testonly = 1,
    hdrs = ["simulated_test_base.h"],
    deps = [
        "//integration:simavr_for_testing",
        "//simulator:simulator_delegate_mock",
        "//simulator/components:usb_host",
        "//util:status_util",
        "@abseil//absl/container:flat_hash_map",
        "@abseil//absl/status",
        "@abseil//absl/strings",
        "@gtest",
        "@gtest//:gtest_main",
    ],
    alwayslink = 1,
)

cc_library(
    name = "simavr_for_testing",
    testonly = 1,
    srcs = ["simavr_for_testing.cpp"],
    hdrs = ["simavr_for_testing.h"],
    deps = [
        "//simulator/simavr:simavr_impl",
        "@abseil//absl/strings",
    ],
    alwayslink = 1,
)
