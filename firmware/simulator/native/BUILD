load("@avr-bazel//:avr.bzl", "avr_binary")

package(default_visibility = ["//visibility:public"])

avr_binary(
    name = "threeboard_sim_binary",
    srcs = ["//src:main.c"],
    copts = select({
        "@bazel_tools//src/conditions:darwin": ["-DDARWIN"],
        "//conditions:default": ["-DLINUX"],
    }) + [
        # This flag enables simavr-specific sections to be added to the
        # firmware file for parsing by simavr.
        "-DTHREEBOARD_SIM",
        "-g",
    ],
    deps = [
        "//src:bootstrap",
        "@third_party//:simavr_avr_hdrs",
    ],
)
