#include "graphql_cpp/utf8_stream_reader.h"

namespace graphql_cpp {

Utf8StreamReader::Utf8StreamReader(std::istream& stream) noexcept
    : stream_(stream) {}

Utf8StreamReader::ReadResult Utf8StreamReader::read() noexcept {
  char first_byte{};
  stream_.get(first_byte);
  if (stream_.good()) {
    return parse_first_byte(static_cast<std::uint8_t>(first_byte));
  } else if (stream_.eof()) {
    position_ += 1;
    return std::nullopt;
  } else if (stream_.bad()) {
    return bad_error();
  } else if (stream_.fail()) {
    return fail_error();
  } else {
    return unexpected_error();
  }
}

Utf8StreamReader::ReadResult Utf8StreamReader::parse_first_byte(
    std::uint8_t first_byte) noexcept {
  if (0x00 <= first_byte && first_byte <= 0x7f) {
    return parse_continuation_bytes_length(first_byte, 0);
  } else if (0xc2 <= first_byte && first_byte <= 0xdf) {
    return parse_continuation_bytes_length(first_byte, 1);
  } else if (0xe0 <= first_byte && first_byte <= 0xef) {
    return parse_continuation_bytes_length(first_byte, 2);
  } else if (0xf0 <= first_byte && first_byte <= 0xf4) {
    return parse_continuation_bytes_length(first_byte, 3);
  } else {
    return AnyError{
        std::format("Invalid UTF-8 first byte: 0x{:02x} at position: {}",
                    first_byte, position_)};
  }
}

Utf8StreamReader::ReadResult Utf8StreamReader::parse_continuation_bytes_length(
    std::uint8_t first_byte, std::size_t continuation_bytes_length) noexcept {
  std::uint32_t cp = first_byte & ((1 << (7 - continuation_bytes_length)) - 1);
  for (std::size_t i = 0; i < continuation_bytes_length; ++i) {
    char byte{};
    stream_.get(byte);
    if (stream_.good()) {
      const std::uint8_t cont_byte = static_cast<std::uint8_t>(byte);
      if (0x80 <= cont_byte && cont_byte <= 0xbf) {
        cp = (cp << 6) | (cont_byte & 0x3f);
      } else {
        return AnyError{std::format(
            "Invalid UTF-8 continuation byte: 0x{:02x} at position: {}",
            cont_byte, position_)};
      }
    } else if (stream_.eof()) {
      return AnyError{
          std::format("Incomplete UTF-8 sequence at position: {}", position_)};
    } else if (stream_.bad()) {
      return bad_error();
    } else if (stream_.fail()) {
      return fail_error();
    } else {
      return unexpected_error();
    }
  }

  return parse_overlong_encoding(cp, continuation_bytes_length + 1);
}

Utf8StreamReader::ReadResult Utf8StreamReader::parse_overlong_encoding(
    std::uint32_t code_point, std::size_t utf8_bytes_length) noexcept {
  if (utf8_bytes_length > 1 && code_point < 0x80 ||
      utf8_bytes_length > 2 && code_point < 0x800 ||
      utf8_bytes_length > 3 && code_point < 0x10000) {
    return AnyError{std::format(
        "Invalid overlong UTF-8 encoding at position: {}", position_)};
  }

  return parse_surrogate_pair(code_point, utf8_bytes_length);
}

Utf8StreamReader::ReadResult Utf8StreamReader::parse_surrogate_pair(
    std::uint32_t code_point, std::size_t utf8_bytes_length) noexcept {
  if (0xd800 <= code_point && code_point <= 0xdfff) {
    return AnyError{std::format(
        "Invalid UTF-8 surrogate pair encoding at position: {}", position_)};
  }

  return parse_code_point_beyond_unicode_range(code_point, utf8_bytes_length);
}

Utf8StreamReader::ReadResult
Utf8StreamReader::parse_code_point_beyond_unicode_range(
    std::uint32_t code_point, std::size_t utf8_bytes_length) noexcept {
  if (code_point > 0x10ffff) {
    return AnyError{std::format(
        "Code point beyond Unicode range at position: {}", position_)};
  }

  position_ += utf8_bytes_length;
  return std::make_optional(code_point);
}

Utf8StreamReader::ReadResult Utf8StreamReader::bad_error() noexcept {
  return AnyError{static_cast<std::int32_t>(stream_.rdstate()),
                  "A fatal error occurred while reading stream"};
}

Utf8StreamReader::ReadResult Utf8StreamReader::fail_error() noexcept {
  return AnyError{static_cast<std::int32_t>(stream_.rdstate()),
                  "A failure occurred while reading stream"};
}

Utf8StreamReader::ReadResult Utf8StreamReader::unexpected_error() noexcept {
  return AnyError{
      static_cast<std::int32_t>(stream_.rdstate()),
      std::format("An unexpected error occurred while reading stream at "
                  "position: {}",
                  position_)};
}

}  // namespace graphql_cpp
