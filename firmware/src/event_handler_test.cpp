#include "event_handler.h"

#include <memory>
#include <thread>

#include "src/native/native_mock.h"

namespace threeboard {
namespace {

using testing::_;
using testing::InvokeWithoutArgs;

class EventHandlerTest : public ::testing::Test {
public:
  EventHandlerTest() {
    handler_ = std::make_unique<EventHandler>(&native_mock_);
  }

  native::NativeMock native_mock_;
  std::unique_ptr<EventHandler> handler_;
};

TEST_F(EventHandlerTest, TestNoDelayWhenPendingKeypressPresent) {
  EXPECT_CALL(native_mock_, Delay(_)).Times(0);
  handler_->HandleKeypress(Keypress::X);
  auto event = handler_->WaitForKeyboardEvent();
  ASSERT_EQ(event, Keypress::X);
}

TEST_F(EventHandlerTest, TestDelayCalledBeforeKeypressPresent) {
  EXPECT_CALL(native_mock_, Delay(1)).Times(1).WillOnce(InvokeWithoutArgs([]() {
    usleep(100);
  }));
  bool has_returned = false;
  std::thread t1([this, &has_returned]() {
    auto event = handler_->WaitForKeyboardEvent();
    ASSERT_EQ(event, Keypress::X);
    has_returned = true;
  });
  usleep(50);

  handler_->HandleKeypress(Keypress::X);
  t1.join();
  ASSERT_TRUE(has_returned);
}
} // namespace
} // namespace threeboard
