package(default_visibility = ["//visibility:public"])

load(
    "@avr-bazel//:avr.bzl",
    "avr_binary",
)

avr_binary(
    name = "threeboard_sim_binary",
    srcs = ["//src:main.c"],
    deps = [
        "@third_party//:simavr_avr_hdrs",
        "//src:bootstrap",
    ],
    copts = select({
        "@bazel_tools//src/conditions:darwin": ["-DDARWIN"],
	"//conditions:default": ["-DLINUX"],
    }) + ["-DTHREEBOARD_SIM", "-g"],
)

cc_library(
    name = "simulator_delegate",
    hdrs = ["simulator_delegate.h"],
)

cc_library(
    name = "simulator_lib",
    hdrs = ["simulator.h"],
    srcs = ["simulator.cpp"],
    deps = [
        "@third_party//:simavr"
    ],
    data = [":threeboard_sim_binary"],
)

cc_library(
    name = "runner",
    hdrs = ["runner.h"],
    srcs = ["runner.cpp"],
    deps = [
        ":simulator_lib",
	":simulator_delegate",
	"//simulator/ui:ui",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = [
        ":runner",
    ],
)
