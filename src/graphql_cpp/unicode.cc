#include "graphql_cpp/unicode.h"

namespace graphql_cpp {

std::optional<std::size_t> Utf8BytesLength(std::uint8_t first_byte) noexcept {
  if (0x00 <= first_byte && first_byte <= 0x7f) {
    return 1;
  } else if (0xc2 <= first_byte && first_byte <= 0xdf) {
    return 2;
  } else if (0xe0 <= first_byte && first_byte <= 0xef) {
    return 3;
  } else if (0xf0 <= first_byte && first_byte <= 0xf4) {
    return 4;
  } else {
    return std::nullopt;
  }
}

std::optional<std::uint32_t> Utf8CodePoint(Slice<std::uint8_t> bytes) noexcept {
  if (bytes.empty() || bytes.size() > 4) {
    return std::nullopt;
  }

  const std::size_t len = bytes.size();
  if (len == 1) {
    return bytes[0];
  }

  std::uint32_t cp = bytes[0] & ((1 << (7 - len)) - 1);
  for (std::size_t i = 1; i < len; ++i) {
    cp = (cp << 6) | (bytes[i] & 0x3f);
  }

  return cp;
}

bool IsUtf8ContinuationByte(std::uint8_t byte) noexcept {
  return 0x80 <= byte && byte <= 0xbf;
}

}  // namespace graphql_cpp
