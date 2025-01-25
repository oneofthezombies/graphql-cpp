#include "graphql_cpp/utf8_stream.h"

#include <cassert>

namespace graphql_cpp {

Utf8Stream::Utf8Stream(std::unique_ptr<std::istream> stream) noexcept
    : stream_(std::move(stream)) {}

Result<std::optional<std::uint32_t>, Utf8Stream::NextError>
Utf8Stream::Next() noexcept {
  auto first_byte_res = ReadByte();
  if (IsErr(first_byte_res)) {
    return std::visit(
        [](auto&& error) -> Result<std::optional<std::uint32_t>, NextError> {
          return error;
        },
        UnwrapErr(first_byte_res));
  }

  auto first_byte_opt = Unwrap(first_byte_res);
  if (!first_byte_opt) {
    return std::nullopt;
  }

  const std::uint8_t first_byte = *first_byte_opt;
  const auto code_point_length_opt = Utf8CodePointLength(first_byte);
  if (!code_point_length_opt) {
    return Utf8FirstByteError{.position = position_, .first_byte = first_byte};
  }

  const std::size_t code_point_length = *code_point_length_opt;
  auto next_code_point_res = NextCodePoint(first_byte, code_point_length);
  if (IsErr(next_code_point_res)) {
    return std::visit(
        [](auto&& error) -> Result<std::optional<std::uint32_t>, NextError> {
          return error;
        },
        UnwrapErr(next_code_point_res));
  }

  position_ += code_point_length;
  return Unwrap(next_code_point_res);
}

Result<std::optional<std::uint8_t>, Utf8Stream::ReadByteError>
Utf8Stream::ReadByte() noexcept {
  char byte{};
  stream_->get(byte);
  if (stream_->good()) {
    return static_cast<std::uint8_t>(byte);
  } else if (stream_->eof()) {
    // Check `eof` before `fail` as both can be set simultaneously.
    return std::nullopt;
  } else if (stream_->bad()) {
    return BadStreamError{.position = position_};
  } else if (stream_->fail()) {
    return FailStreamError{.position = position_};
  } else {
    return UnexpectedStreamError{.position = position_};
  }
}

Result<std::optional<std::uint32_t>, Utf8Stream::NextError>
Utf8Stream::NextCodePoint(std::uint8_t first_byte,
                          std::size_t code_point_length) noexcept {
  assert(1 <= code_point_length && code_point_length <= 4);
  if (code_point_length == 1) {
    return first_byte;
  }

  std::uint8_t code_point_bytes[4] = {first_byte, 0, 0, 0};
  for (std::size_t i = 1; i < code_point_length; ++i) {
    auto read_res = ReadByte();
    if (IsErr(read_res)) {
      return std::visit(
          [](auto&& error) -> Result<std::optional<std::uint32_t>, NextError> {
            return error;
          },
          UnwrapErr(read_res));
    }

    auto byte_opt = Unwrap(read_res);
    if (!byte_opt) {
      return Utf8CodePointLengthError{.position = position_,
                                      .code_point_length = code_point_length};
    }

    const std::uint8_t byte = *byte_opt;
    if (IsUtf8ContinuationByte(byte)) {
      code_point_bytes[i] = byte;
    } else {
      return Utf8ContinuationByteError{.position = position_ + i,
                                       .continuation_byte = byte};
    }
  }

  const auto code_point_opt =
      Utf8CodePoint(code_point_bytes, code_point_length);
  // `Utf8CodePoint` returns a valid code point.
  assert(code_point_opt);
  return *code_point_opt;
}

}  // namespace graphql_cpp
