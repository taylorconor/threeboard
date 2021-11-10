load("@rules_cc//cc:defs.bzl", "cc_library")

cc_test(
    name = "integration_test",
    srcs = ["integration_test.cpp"],
    deps = [
        "//integration/util:fake_state_storage",
        "//integration/util:instrumenting_simavr",
        "//simulator:simulator_lib",
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
    env = {"RC_PARAMS": "max_success=50 max_size=750 noshrink=1"},
    deps = [
        "//integration/model:rapidcheck_specializations",
        "//integration/model:threeboard_model",
        "//integration/util:fake_state_storage",
        "//integration/util:testable_simavr",
        "//simulator:simulator_lib",
        "//util:gtest_util",
        "@emil-e//:rapidcheck",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
