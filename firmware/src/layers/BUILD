load("@avr-bazel//:avr.bzl", "avr_binary", "avr_hex", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "layer_id",
    hdrs = ["layer_id.h"],
)

avr_library(
    name = "layer",
    hdrs = ["layer.h"],
    deps = [
        "//src/delegates:keypress_handler_delegate",
    ],
)

avr_library(
    name = "default_layer",
    hdrs = ["default_layer.h"],
    srcs = ["default_layer.cpp"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)

avr_library(
    name = "layer_r",
    hdrs = ["layer_r.h"],
    srcs = ["layer_r.cpp"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)

avr_library(
    name = "layer_g",
    hdrs = ["layer_g.h"],
    srcs = ["layer_g.cpp"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)

avr_library(
    name = "layer_b",
    hdrs = ["layer_b.h"],
    srcs = ["layer_b.cpp"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)