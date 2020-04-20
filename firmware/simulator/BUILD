load(
    "@avr-bazel//:avr.bzl",
    "avr_pure_library",
    "avr_binary",
)

avr_binary(
    name = "threeboard_sim_binary",
    srcs = ["//src:main.c"],
    deps = [
        "@third_party//:simavr_avr_hdrs",
        "//src:bootstrap",
    ],
    copts = ["-DTHREEBOARD_SIM"],
)

cc_binary(
    name = "simulator",
    srcs = ["main.cpp"],
    deps = ["@third_party//:simavr"],
    data = [":threeboard_sim_binary"],
)
