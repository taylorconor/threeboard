load("@avr-bazel//:avr.bzl", "avr_binary", "avr_hex", "avr_library", "avr_pure_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "layer_id",
    hdrs = ["layer_id.h"],
)

avr_library(
    name = "layer_controller",
    srcs = ["layer_controller.cpp"],
    hdrs = ["layer_controller.h"],
    deps = [
        ":default_layer",
        ":layer_b",
        ":layer_g",
        ":layer_r",
        "//src/delegates:layer_controller_delegate",
        "//src/usb",
    ],
)

avr_library(
    name = "layer",
    srcs = ["layer.cpp"],
    hdrs = ["layer.h"],
    deps = [
        "//src:led_controller",
        "//src/delegates:keypress_handler_delegate",
        "//src/layers:layer_id",
        "//src/usb",
    ],
)

avr_library(
    name = "default_layer",
    srcs = ["default_layer.cpp"],
    hdrs = ["default_layer.h"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)

avr_library(
    name = "layer_r",
    srcs = ["layer_r.cpp"],
    hdrs = ["layer_r.h"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)

avr_library(
    name = "layer_g",
    srcs = ["layer_g.cpp"],
    hdrs = ["layer_g.h"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)

avr_library(
    name = "layer_b",
    srcs = ["layer_b.cpp"],
    hdrs = ["layer_b.h"],
    deps = [
        ":layer",
        "//src:led_controller",
        "//src/delegates:layer_controller_delegate",
    ],
)
