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
    deps = [
        "//integration/model:threeboard_model",
        "//integration/util:fake_state_storage",
        "//integration/util:testable_simavr",
        "//simulator:simulator_lib",
        "//util:gtest_util",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
