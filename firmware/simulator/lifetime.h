#pragma once

#include <functional>
#include <utility>

namespace threeboard {
namespace simulator {

// A utility class to represent lifetime of an object, and provide a cleanup
// function to be called on destruction.
class Lifetime {
 public:
  explicit Lifetime(std::function<void()> cleanup)
      : cleanup_(std::move(cleanup)) {}

  ~Lifetime() { cleanup_(); }

 private:
  std::function<void()> cleanup_;
};
}  // namespace simulator
}  // namespace threeboard
