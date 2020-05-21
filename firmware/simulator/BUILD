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
        "@bazel_tools//src/conditions:darwin": ["-DDARWIN", "-DTHREEBOARD_SIM"],
	"//conditions:default": ["-DLINUX", "-DTHREEBOARD_SIM"],
    }),
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
    name = "sim_ui",
    hdrs = ["sim_ui.h"],
    srcs = ["sim_ui.cpp"],
    linkopts = ["-lcurses"],
)

cc_library(
    name = "sim_runner",
    hdrs = ["sim_runner.h"],
    srcs = ["sim_runner.cpp"],
    deps = [
        ":simulator_lib",
	":sim_ui",
    ],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = [
        ":sim_runner",
    ],
)
