package(default_visibility = [
    "//integration:__subpackages__",
    "//simulator:__subpackages__",
])

cc_library(
    name = "instrumenting_simavr",
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
