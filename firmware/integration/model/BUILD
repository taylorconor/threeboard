load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = [
    "//integration:__subpackages__",
])

cc_library(
    name = "layer_model",
    srcs = ["layer_model.cpp"],
    hdrs = ["layer_model.h"],
    deps = [
        "//simulator:simulator_state",
        "//simulator/components:usb_keycodes",
        "//src:keypress",
        "//src:led_state",
        "//src/storage:storage_controller",
    ],
)

cc_library(
    name = "threeboard_model",
    srcs = ["threeboard_model.cpp"],
    hdrs = ["threeboard_model.h"],
    deps = [
        ":layer_model",
        "//src:keypress",
        "//src/layers:layer_id",
    ],
)
