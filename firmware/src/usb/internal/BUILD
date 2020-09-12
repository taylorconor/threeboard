load(
    "@avr-bazel//:avr.bzl",
    "avr_library",
)

package(default_visibility = ["//src/usb:__subpackages__"])

avr_library(
    name = "hid_state",
    hdrs = ["hid_state.h"],
)

avr_library(
    name = "descriptors",
    srcs = ["descriptors.cpp"],
    hdrs = ["descriptors.h"],
    deps = [
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
        "//src/util",
    ],
)

avr_library(
    name = "request_handler",
    srcs = ["request_handler.cpp"],
    hdrs = ["request_handler.h"],
    deps = [
        ":descriptors",
        ":hid_state",
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
        "//src/native",
        "//src/util",
    ],
)

cc_library(
    name = "request_handler_mock",
    testonly = 1,
    hdrs = ["request_handler_mock.h"],
    deps = [
        ":request_handler",
    ],
)
