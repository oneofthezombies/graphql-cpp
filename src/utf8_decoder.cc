#include "utf8_decoder.h"

#include <optional>

namespace graphql_cpp {

namespace utf8_decoder {

std::optional<std::size_t> Utf8CodePointLength(
    const std::uint8_t first_byte) noexcept {
  if (0x00 <= first_byte && first_byte <= 0x7f) {
    return 1u;
  } else if (0xc2 <= first_byte && first_byte <= 0xdf) {
    return 2u;
  } else if (0xe0 <= first_byte && first_byte <= 0xef) {
    return 3u;
  } else if (0xf0 <= first_byte && first_byte <= 0xf4) {
    return 4u;
  } else {
    return std::nullopt;
  }
}

Utf8Decoder::Utf8Decoder(const slice::Slice<std::uint8_t> text) noexcept
    : text_(text) {}

NextCodePointResult Utf8Decoder::NextCodePoint() noexcept {
  if (position_ >= text_.size()) {
    return Eof{};
  }

  const std::uint8_t first_byte = text_[position_];
  const auto code_point_length_opt = Utf8CodePointLength(first_byte);
  if (!code_point_length_opt) {
    return FirstByteError{
        .position = position_,
        .first_byte = first_byte,
    };
  }

  const std::size_t code_point_length = *code_point_length_opt;
  if (position_ + code_point_length > text_.size()) {
    return CodePointLengthError{
        .position = position_,
        .code_point_length = code_point_length,
        .text_length = text_.size(),
    };
  }

  std::uint32_t code_point = 0;
  if (code_point_length == 1) {
    code_point = text_[position_];
  } else {
    code_point = text_[position_] & ((1 << (7 - code_point_length)) - 1);
    for (std::size_t i = 1; i < code_point_length; ++i) {
      const std::uint8_t continuation_byte = text_[position_ + i];
      if (0x80 <= continuation_byte && continuation_byte <= 0xbf) {
        code_point = (code_point << 6) | (continuation_byte & 0x3f);
      } else {
        return ContinuationByteError{
            .position = position_ + i,
            .continuation_byte = continuation_byte,
        };
      }
    }
  }

  position_ += code_point_length;
  return CodePoint{.value = code_point};
}

}  // namespace utf8_decoder

}  // namespace graphql_cpp
