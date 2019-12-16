load(
  "//toolchain:avr.bzl",
  "avr_pure_library",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "native",
  hdrs = ["native.h"],
)

avr_pure_library(
  name = "native_impl",
  srcs = ["native_impl.cpp"],
  hdrs = ["native_impl.h"],
  deps = [":native"],
)

cc_library(
  name = "native_mock",
  hdrs = ["native_mock.h"],
  deps = [
    "@gtest//:gtest",
    ":native",
  ],
)
