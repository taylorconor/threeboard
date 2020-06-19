package(default_visibility = ["//visibility:public"])

cc_library(
    name = "key",
    hdrs = ["key.h"],
)

cc_library(
    name = "ui",
    hdrs = ["ui.h"],
    srcs = ["ui.cpp"],
    deps = [
        ":key",
	"//simulator:simulator_delegate",
    ],
    linkopts = ["-lcurses"],
)

