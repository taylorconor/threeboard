#include "usb_keycodes.h"

#include <cctype>
#include <tuple>

namespace threeboard {
namespace simulator {

std::pair<char, uint8_t> ToUsbKeycodes(char c) {
  char keycode = 0;
  uint8_t modcode = 0;
  if (std::isalpha(c)) {
    if (std::isupper(c)) {
      modcode = (1 << 1);
    }
    keycode = std::tolower(c) - 'a' + 4;
  } else if (std::isdigit(c)) {
    keycode = std::tolower(c) - '0' + 0x1e;
  } else if (std::isspace(c)) {
    keycode = 0x2c;
  } else if (std::ispunct(c)) {
    modcode = (1 << 1);
    if (c == '!') {
      keycode = 0x1e;
    } else if (c == '@') {
      keycode = 0x1f;
    } else if (c == '#') {
      keycode = 0x20;
    } else if (c == '$') {
      keycode = 0x21;
    } else if (c == '%') {
      keycode = 0x22;
    } else if (c == '^') {
      keycode = 0x23;
    } else if (c == '&') {
      keycode = 0x24;
    } else if (c == '&') {
      keycode = 0x25;
    } else if (c == '*') {
      keycode = 0x26;
    } else if (c == '(') {
      keycode = 0x27;
    } else if (c == ')') {
      keycode = 0x28;
    } else if (c == '-') {
      keycode = 0x2d;
      modcode = 0;
    } else if (c == '_') {
      keycode = 0x2d;
    } else if (c == '=') {
      keycode = 0x2e;
      modcode = 0;
    } else if (c == '+') {
      keycode = 0x2e;
    }
  }
  return std::tie(keycode, modcode);
}

char FromUsbKeycodes(char keycode, uint8_t modcode) {
  char c = 0;
  const bool is_shift = modcode & (1 << 1);
  if (keycode >= 0x04 && keycode <= 0x1d) {
    c = keycode - 0x04 + 'a';
    if (is_shift) {
      c = std::toupper(c);
    }
  } else if (keycode >= 0x1e && keycode <= 0x27) {
    if (!is_shift) {
      c = keycode - 0x1e + '1';
    } else if (keycode == 0x1e) {
      c = '!';
    } else if (keycode == 0x1f) {
      c = '@';
    } else if (keycode == 0x20) {
      c = '#';
    } else if (keycode == 0x21) {
      c = '$';
    } else if (keycode == 0x22) {
      c = '%';
    } else if (keycode == 0x23) {
      c = '^';
    } else if (keycode == 0x24) {
      c = '&';
    } else if (keycode == 0x25) {
      c = '*';
    } else if (keycode == 0x26) {
      c = '(';
    } else if (keycode == 0x27) {
      c = ')';
    }
  } else if (keycode == 0x2c) {
    c = ' ';
  } else if (keycode == 0x2d) {
    c = is_shift ? '_' : '-';
  } else if (keycode == 0x2e) {
    c = is_shift ? '+' : '=';
  }
  return c;
}

}  // namespace simulator
}  // namespace threeboard