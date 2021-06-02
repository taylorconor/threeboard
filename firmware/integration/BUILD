load("@rules_cc//cc:defs.bzl", "cc_library")

cc_test(
    name = "integration_test",
    srcs = ["integration_test.cpp"],
    deps = [
        "//simulator:simulator_delegate_mock",
        "//simulator/components:usb_host",
        "//simulator/simavr:instrumenting_simavr",
        "//util:gtest_util",
        "//util:status_util",
        "@abseil//absl/container:flat_hash_map",
        "@abseil//absl/status",
        "@abseil//absl/strings",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
