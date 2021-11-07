load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "simulator_delegate",
    hdrs = ["simulator_delegate.h"],
    deps = ["//simulator/util:flags"],
)

cc_library(
    name = "simulator_delegate_mock",
    hdrs = ["simulator_delegate_mock.h"],
    deps = [
        ":simulator_delegate",
        "@gtest",
    ],
)

cc_library(
    name = "simulator_lib",
    srcs = ["simulator.cpp"],
    hdrs = ["simulator.h"],
    deps = [
        ":simulator_delegate",
        ":simulator_state",
        "//simulator/components:i2c_eeprom",
        "//simulator/components:uart",
        "//simulator/components:usb_host_impl",
        "//simulator/simavr",
        "//simulator/util:flags",
        "//simulator/util:state_storage",
        "//src:keypress",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    linkopts = select({
        "@bazel_tools//src/conditions:darwin": [
            # macOS experiences strange runtime crashes without linking
            # Foundation. Seems to be related to Bazel's complex compiler
            # toolchain setup (external/local_config_cc/cc_wrapper.sh), but I
            # haven't spent too much time looking into it.
            "-framework Foundation",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":simulator_lib",
        "//simulator/simavr:simavr_impl",
        "//simulator/ui",
        "//simulator/util:flags",
        "//simulator/util:logging",
        "//simulator/util:state_storage_impl",
        "//src:keypress",
    ],
)

cc_library(
    name = "simulator_state",
    hdrs = ["simulator_state.h"],
)
