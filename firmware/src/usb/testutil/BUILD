load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//src/usb:__subpackages__"])

cc_library(
    name = "testutil",
    testonly = 1,
    srcs = ["testutil.cpp"],
    hdrs = ["testutil.h"],
    deps = [
        "//src/native:native_mock",
        "//src/usb/shared:protocol",
    ],
)

cc_library(
    name = "fake_host",
    testonly = 1,
    srcs = ["fake_host.cpp"],
    hdrs = ["fake_host.h"],
    deps = [
        "//src/native:native_mock",
        "//src/usb:usb_controller_impl",
        "//src/usb/shared:protocol",
    ],
)
