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
