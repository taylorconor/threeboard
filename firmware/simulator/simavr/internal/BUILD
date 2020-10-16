package(default_visibility = ["//simulator/simavr:__subpackages__"])

cc_library(
    name = "sim_core_declare",
    hdrs = ["sim_core_declare.h"],
)

cc_library(
    name = "sim_32u4",
    hdrs = [
        "iom32u4.h",
        "sim_32u4.h",
        "sim_core_declare.h",
    ],
    deps = [
        ":sim_core_declare",
        "//src/native:constants",
    ],
)
