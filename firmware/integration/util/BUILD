package(default_visibility = [
    "//integration:__subpackages__",
    "//simulator:__subpackages__",
])

cc_library(
    name = "instrumenting_simavr",
    srcs = ["instrumenting_simavr.cpp"],
    hdrs = ["instrumenting_simavr.h"],
    data = ["//simulator/native:threeboard_sim_fast_binary"],
    deps = [
        "//simulator/simavr:simavr_impl",
        "//util:status_util",
        "@abseil//absl/container:flat_hash_map",
        "@abseil//absl/status",
        "@abseil//absl/strings",
    ],
)

cc_library(
    name = "fake_state_storage",
    hdrs = ["fake_state_storage.h"],
    deps = ["//simulator/util:state_storage"],
)
