load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "layer_model",
    srcs = ["layer_model.cpp"],
    hdrs = ["layer_model.h"],
    deps = [
        ":observable_state",
        "//src:keypress",
        "//src:led_state",
    ],
)

cc_library(
    name = "observable_state",
    hdrs = ["observable_state.h"],
    deps = ["//src:led_state"],
)

cc_library(
    name = "threeboard_model",
    srcs = ["threeboard_model.cpp"],
    hdrs = ["threeboard_model.h"],
    deps = [
        ":layer_model",
        ":observable_state",
        "//src:keypress",
        "//src/layers:layer_id",
    ],
)
