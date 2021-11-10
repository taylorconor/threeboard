load("@avr-bazel//:avr.bzl", "avr_binary")

package(default_visibility = ["//visibility:public"])

avr_binary(
    name = "threeboard_sim_realtime_binary",
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

avr_binary(
    name = "threeboard_sim_fast_binary",
    srcs = ["//src:main.c"],
    copts = select({
        "@bazel_tools//src/conditions:darwin": ["-DDARWIN"],
        "//conditions:default": ["-DLINUX"],
    }) + [
        # This flag enables simavr-specific sections to be added to the
        # firmware file for parsing by simavr.
        "-DTHREEBOARD_SIM",
        # Override the CPU clock speed to 160 MHz instead of the default
        # 16 MHz. Headless simulated tests can run faster without adverse
        # effects by running the firmware faster than realtime.
        "-DF_CPU_OVERRIDE=160000000",
        "-g",
    ],
    deps = [
        "//src:bootstrap",
        "@third_party//:simavr_avr_hdrs",
    ],
)
