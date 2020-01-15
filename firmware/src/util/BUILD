load(
  "//toolchain:avr.bzl",
  "avr_library",
)

package(default_visibility = ["//visibility:public"])

avr_library(
  name = "bitset",
  hdrs = ["bitset.h"],
)

cc_test(
  name = "bitset_test",
  srcs = ["bitset_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    ":bitset"
  ],
)

avr_library(
  name = "enum",
  hdrs = ["enum.h"],
)

cc_test(
  name = "enum_test",
  srcs = ["enum_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    ":enum"
  ],
)

avr_library(
  name = "common",
  hdrs = ["common.h"],
)
