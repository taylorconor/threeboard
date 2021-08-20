#pragma once

#include "gmock/gmock.h"
#include "layer_controller.h"

namespace threeboard {
namespace detail {

class DefaultLayerControllerMock : public LayerController {
 public:
  DefaultLayerControllerMock() : LayerController(nullptr, nullptr, nullptr) {}

  MOCK_METHOD(bool, HandleEvent, (const Keypress &), (override));
};
}  // namespace detail

using LayerControllerMock =
    ::testing::StrictMock<detail::DefaultLayerControllerMock>;

}  // namespace threeboard