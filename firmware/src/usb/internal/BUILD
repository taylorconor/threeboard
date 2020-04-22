load(
    "@avr-bazel//:avr.bzl",
    "avr_library",
)

package(default_visibility = ["//src/usb:__subpackages__"])

avr_library(
    name = "constants",
    hdrs = ["constants.h"],
)

avr_library(
    name = "hid_state",
    hdrs = ["hid_state.h"],
)

avr_library(
    name = "protocol",
    srcs = ["protocol.cpp"],
    hdrs = ["protocol.h"],
    deps = [
        "//src/native",
        "//src/util",
    ],
)

avr_library(
    name = "descriptors",
    srcs = ["descriptors.cpp"],
    hdrs = ["descriptors.h"],
    deps = [
        ":constants",
        ":protocol",
        "//src/util",
    ],
)

avr_library(
    name = "request_handler",
    srcs = ["request_handler.cpp"],
    hdrs = ["request_handler.h"],
    deps = [
        ":constants",
        ":descriptors",
        ":hid_state",
        ":protocol",
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
