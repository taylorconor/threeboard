#pragma once

#include "gmock/gmock.h"
#include "layer.h"

namespace threeboard {
namespace detail {

class DefaultLayerMock : public Layer {
 public:
  DefaultLayerMock() : Layer(&led_state, nullptr) {}

  MOCK_METHOD(bool, HandleEvent, (const Keypress &), (override));
  MOCK_METHOD(bool, TransitionedToLayer, (), (override));
  MOCK_METHOD(void, SendToHost, (uint8_t key, uint8_t mod), (override));

  LedState led_state;
};
}  // namespace detail

using LayerMock = ::testing::StrictMock<detail::DefaultLayerMock>;

}  // namespace threeboard