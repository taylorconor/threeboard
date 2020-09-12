package(default_visibility = ["//visibility:public"])

cc_library(
    name = "host",
    srcs = ["host.cpp"],
    hdrs = ["host.h"],
    deps = [
        "@third_party//:simavr",
	"//simulator/core:sim_32u4",
	"//src/usb/shared:constants",
	"//src/usb/shared:protocol",
    ],
)
