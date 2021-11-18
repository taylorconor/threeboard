#include "storage_controller.h"

#include "gtest/gtest.h"
#include "src/storage/internal/eeprom_mock.h"
#include "src/usb/usb_controller_mock.h"

namespace threeboard {
namespace storage {

using testing::_;
using testing::DoAll;
using testing::Return;
using testing::Sequence;
using testing::SetArgPointee;

class StorageControllerTest : public ::testing::Test {
 public:
  StorageControllerTest() {
    auto *raw_ptr =
        new StorageController(&usb_controller_mock_, &internal_eeprom_mock_,
                              &eeprom0_mock_, &eeprom1_mock_);
    storage_controller_ = std::unique_ptr<StorageController>(raw_ptr);
  }

  std::unique_ptr<StorageController> storage_controller_;
  usb::UsbControllerMock usb_controller_mock_;
  EepromMock internal_eeprom_mock_;
  EepromMock eeprom0_mock_;
  EepromMock eeprom1_mock_;
};

namespace {

TEST_F(StorageControllerTest, SetCharacterShortcutSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0, 10)).WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->SetCharacterShortcut(0, 10));
}

TEST_F(StorageControllerTest, SetCharacterShortcutFailure) {
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0, 10)).WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SetCharacterShortcut(0, 10));
}

TEST_F(StorageControllerTest, GetCharacterShortcutSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  uint8_t byte = 0;
  EXPECT_TRUE(storage_controller_->GetCharacterShortcut(0, &byte));
  EXPECT_EQ(byte, 10);
}

TEST_F(StorageControllerTest, GetCharacterShortcutFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0, _)).WillOnce(Return(false));
  uint8_t byte = 0;
  EXPECT_FALSE(storage_controller_->GetCharacterShortcut(0, &byte));
  EXPECT_EQ(byte, 0);
}

TEST_F(StorageControllerTest, AppendToWordShortcutSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, WriteByte((16 * 4) + 10, 100))
      .WillOnce(Return(true));
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x100 + 4, 11))
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->AppendToWordShortcut(4, 100));
}

TEST_F(StorageControllerTest, AppendToWordShortcutFailsOnGetLengthFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToWordShortcut(4, 100));
}

TEST_F(StorageControllerTest, AppendToWordShortcutFailsWhenFull) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(15), Return(true)));
  EXPECT_FALSE(storage_controller_->AppendToWordShortcut(4, 100));
}

TEST_F(StorageControllerTest, AppendToWordShortcutFailsOnShortcutWriteFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, WriteByte((16 * 4) + 10, 100))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToWordShortcut(4, 100));
}

TEST_F(StorageControllerTest, AppendToWordShortcutFailsOnLengthWriteFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, WriteByte((16 * 4) + 10, 100))
      .WillOnce(Return(true));
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x100 + 4, 11))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToWordShortcut(4, 100));
}

TEST_F(StorageControllerTest, ClearWordShortcutSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x100 + 4, 0))
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->ClearWordShortcut(4));
}

TEST_F(StorageControllerTest, ClearWordShortcutFailure) {
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x100 + 4, 0))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->ClearWordShortcut(4));
}

TEST_F(StorageControllerTest, GetWordShortcutLengthSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  uint8_t length = 0;
  EXPECT_TRUE(storage_controller_->GetWordShortcutLength(4, &length));
  EXPECT_EQ(length, 10);
}

TEST_F(StorageControllerTest, GetWordShortcutLengthFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(Return(false));
  uint8_t length = 0;
  EXPECT_FALSE(storage_controller_->GetWordShortcutLength(4, &length));
  EXPECT_EQ(length, 0);
}

TEST_F(StorageControllerTest, SendWordShortcutSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  Sequence seq;
  for (int i = 0; i < 10; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte((16 * 4) + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, 0))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 0));
}

TEST_F(StorageControllerTest, SendWordShortcutSuccessUppercase) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  Sequence seq;
  for (int i = 0; i < 10; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte((16 * 4) + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, (1 << 1)))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 1));
}

TEST_F(StorageControllerTest, SendWordShortcutSuccessCapitalise) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  Sequence seq;
  for (int i = 0; i < 10; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte((16 * 4) + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    uint8_t modcode = (i == 0) ? (1 << 1) : 0;
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, modcode))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 2));
}

TEST_F(StorageControllerTest, SendWordShortcutSuccessAppendPeriod) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  Sequence seq;
  for (int i = 0; i < 10; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte((16 * 4) + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, 0))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0x37, 0))
      .InSequence(seq)
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 3));
}

TEST_F(StorageControllerTest, SendWordShortcutSuccessAppendComma) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  Sequence seq;
  for (int i = 0; i < 10; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte((16 * 4) + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, 0))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0x36, 0))
      .InSequence(seq)
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 4));
}

TEST_F(StorageControllerTest, SendWordShortcutSuccessAppendHyphen) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  Sequence seq;
  for (int i = 0; i < 10; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte(64 + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, 0))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0x2d, 0))
      .InSequence(seq)
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 5));
}

TEST_F(StorageControllerTest, SendWordShortcutSuccessAppendToFullShortcut) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(15), Return(true)));
  Sequence seq;
  for (int i = 0; i < 15; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte(64 + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, 0))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0x2d, 0))
      .InSequence(seq)
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->SendWordShortcut(4, 5));
}

TEST_F(StorageControllerTest, SendWordShortcutFailsOnLengthReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendWordShortcut(4, 5));
}

TEST_F(StorageControllerTest, SendWordShortcutFailsOnShortcutReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, ReadByte(64, _)).WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendWordShortcut(4, 5));
}

TEST_F(StorageControllerTest, SendWordShortcutFailsOnUsbSendFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(15), Return(true)));
  EXPECT_CALL(eeprom0_mock_, ReadByte(64, _))
      .WillOnce(DoAll(SetArgPointee<1>(0), Return(true)));
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0, 0)).WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendWordShortcut(4, 5));
}

TEST_F(StorageControllerTest, SendWordShortcutFailsOnUsbSendAppendFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x100 + 4, _))
      .WillOnce(DoAll(SetArgPointee<1>(15), Return(true)));
  Sequence seq;
  for (int i = 0; i < 15; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte(64 + i, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, 0))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0x2d, 0))
      .InSequence(seq)
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendWordShortcut(4, 5));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutSuccessEeprom0) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, WriteByte(0x1000 + (119 * 512) + (10 * 2), 100))
      .WillOnce(Return(true));
  EXPECT_CALL(eeprom0_mock_,
              WriteByte(0x1000 + (119 * 512) + (10 * 2) + 1, 101))
      .WillOnce(Return(true));
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x200 + 119, 11))
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->AppendToBlobShortcut(119, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutSuccessEeprom1) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2), 100)).WillOnce(Return(true));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2) + 1, 101))
      .WillOnce(Return(true));
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x200 + 120, 11))
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->AppendToBlobShortcut(120, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsWhenFull) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(255), Return(true)));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(120, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnInvalidShortcutId) {
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(249, 0, 0));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnLengthReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(119, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnLengthWriteFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2), 100)).WillOnce(Return(true));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2) + 1, 101))
      .WillOnce(Return(true));
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x200 + 120, 11))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(120, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnFirstEeprom0Write) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, WriteByte(0x1000 + (119 * 512) + (10 * 2), 100))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(119, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnSecondEeprom0Write) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom0_mock_, WriteByte(0x1000 + (119 * 512) + (10 * 2), 100))
      .WillOnce(Return(true));
  EXPECT_CALL(eeprom0_mock_,
              WriteByte(0x1000 + (119 * 512) + (10 * 2) + 1, 101))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(119, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnFirstEeprom1Write) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2), 100)).WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(120, 100, 101));
}

TEST_F(StorageControllerTest, AppendToBlobShortcutFailsOnSecondEeprom1Write) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(10), Return(true)));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2), 100)).WillOnce(Return(true));
  EXPECT_CALL(eeprom1_mock_, WriteByte((10 * 2) + 1, 101))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->AppendToBlobShortcut(120, 100, 101));
}

TEST_F(StorageControllerTest, ClearBlobShortcutSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x200 + 4, 0))
      .WillOnce(Return(true));
  EXPECT_TRUE(storage_controller_->ClearBlobShortcut(4));
}

TEST_F(StorageControllerTest, ClearBlobShortcutFailsOnLengthWriteFailure) {
  EXPECT_CALL(internal_eeprom_mock_, WriteByte(0x200 + 4, 0))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->ClearBlobShortcut(4));
}

TEST_F(StorageControllerTest, ClearBlobShortcutFailsOnInvalidShortcutId) {
  EXPECT_FALSE(storage_controller_->ClearBlobShortcut(249));
}

TEST_F(StorageControllerTest, GetBlobShortcutLengthSuccess) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 10, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  uint8_t length = 0;
  EXPECT_TRUE(storage_controller_->GetBlobShortcutLength(10, &length));
  EXPECT_EQ(length, 101);
}

TEST_F(StorageControllerTest, GetBlobShortcutLengthFailsOnLengthReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 10, _))
      .WillOnce(Return(false));
  uint8_t length = 0;
  EXPECT_FALSE(storage_controller_->GetBlobShortcutLength(10, &length));
  EXPECT_EQ(length, 0);
}

TEST_F(StorageControllerTest, GetBlobShortcutLengthFailsOnInvalidShortcutId) {
  uint8_t length = 0;
  EXPECT_FALSE(storage_controller_->GetBlobShortcutLength(249, &length));
  EXPECT_EQ(length, 0);
}

TEST_F(StorageControllerTest, SendBlobShortcutSuccessEeprom0) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  Sequence seq;
  for (int i = 0; i < 101; ++i) {
    EXPECT_CALL(eeprom0_mock_, ReadByte(0x1000 + (119 * 512) + (i * 2), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(eeprom0_mock_, ReadByte(0x1000 + (119 * 512) + (i * 2) + 1, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i + 1), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, i + 1))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_TRUE(storage_controller_->SendBlobShortcut(119));
}

TEST_F(StorageControllerTest, SendBlobShortcutSuccessEeprom1) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  Sequence seq;
  for (int i = 0; i < 101; ++i) {
    EXPECT_CALL(eeprom1_mock_, ReadByte(i * 2, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i), Return(true)));
    EXPECT_CALL(eeprom1_mock_, ReadByte((i * 2) + 1, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(i + 1), Return(true)));
    EXPECT_CALL(usb_controller_mock_, SendKeypress(i, i + 1))
        .InSequence(seq)
        .WillOnce(Return(true));
  }
  EXPECT_TRUE(storage_controller_->SendBlobShortcut(120));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnInvalidShortcutId) {
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(249));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnLengthReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(119));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnEeprom0KeyReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  EXPECT_CALL(eeprom0_mock_, ReadByte(0x1000 + (119 * 512), _))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(119));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnEeprom0ModReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 119, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  EXPECT_CALL(eeprom0_mock_, ReadByte(0x1000 + (119 * 512), _))
      .WillOnce(Return(true));
  EXPECT_CALL(eeprom0_mock_, ReadByte(0x1000 + (119 * 512) + 1, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(119));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnEeprom1KeyReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  EXPECT_CALL(eeprom1_mock_, ReadByte(0, _)).WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(120));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnEeprom1ModReadFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  EXPECT_CALL(eeprom1_mock_, ReadByte(0, _)).WillOnce(Return(true));
  EXPECT_CALL(eeprom1_mock_, ReadByte(1, _)).WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(120));
}

TEST_F(StorageControllerTest, SendBlobShortcutFailsOnUsbSendFailure) {
  EXPECT_CALL(internal_eeprom_mock_, ReadByte(0x200 + 120, _))
      .WillOnce(DoAll(SetArgPointee<1>(101), Return(true)));
  EXPECT_CALL(eeprom1_mock_, ReadByte(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(123), Return(true)));
  EXPECT_CALL(eeprom1_mock_, ReadByte(1, _))
      .WillOnce(DoAll(SetArgPointee<1>(456), Return(true)));
  EXPECT_CALL(usb_controller_mock_, SendKeypress(123, 456))
      .WillOnce(Return(false));
  EXPECT_FALSE(storage_controller_->SendBlobShortcut(120));
}

}  // namespace
}  // namespace storage
}  // namespace threeboard