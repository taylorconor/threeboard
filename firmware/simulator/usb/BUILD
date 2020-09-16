package(default_visibility = ["//visibility:public"])

cc_library(
    name = "host",
    srcs = ["host.cpp"],
    hdrs = ["host.h"],
    deps = [
        "//simulator:simulator_delegate",
        "//simulator/core:sim_32u4",
        "//src/usb/shared:constants",
        "//src/usb/shared:protocol",
        "@third_party//:simavr",
    ],
)
