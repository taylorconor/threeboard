load("@rules_cc//cc:defs.bzl", "cc_library")

cc_test(
    name = "integration_test",
    srcs = ["integration_test.cpp"],
    deps = [
        "//simulator:simulator_delegate_mock",
        "//simulator/components:usb_host",
        "//simulator/simavr:instrumenting_simavr",
        "//util:gtest_util",
        "@abseil//absl/container:flat_hash_map",
        "@abseil//absl/status",
        "@abseil//absl/strings",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "property_test",
    srcs = ["property_test.cpp"],
    env = {"RC_PARAMS": "max_success=3"},
    deps = [
        "//integration/model:keypress_generator",
        "//integration/model:threeboard_model",
        "//simulator:simulator_lib",
        "//simulator/simavr:simavr_impl",
        "@emil-e//:rapidcheck",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
