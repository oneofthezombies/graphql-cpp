#include "graphql_cpp/utf8_bytes_stream.h"

#include "graphql_cpp/unicode.h"

namespace graphql_cpp {

std::expected<std::optional<std::uint8_t>, ReadByteErrorKind> ReadByte(
    std::istream& stream) noexcept {
  char byte{};
  stream.get(byte);
  if (stream.good()) {
    return static_cast<std::uint8_t>(byte);
  } else if (stream.eof()) {
    // Check `eof` before `fail` as both can be set simultaneously.
    return std::nullopt;
  } else if (stream.bad()) {
    return std::unexpected(ReadByteErrorKind::Bad);
  } else if (stream.fail()) {
    return std::unexpected(ReadByteErrorKind::Fail);
  } else {
    return std::unexpected(ReadByteErrorKind::Unexpected);
  }
}

Utf8BytesStream::Utf8BytesStream(std::unique_ptr<std::istream> stream) noexcept
    : stream_(std::move(stream)) {}

Result<std::optional<Utf8BytesStream::Value>, Utf8BytesStream::Error>
Utf8BytesStream::Next() noexcept {
  auto a = ReadByte(*stream_)->and_then([this](auto first_byte_opt) {
    if (!first_byte_opt) {
      return std::nullopt;
    }

    const std::uint8_t first_byte = *first_byte_opt;
  });

  // if (auto first_byte_res = ReadByte(*stream_); first_byte_res) {
  //   if (auto first_byte_res = ReadByte(*stream_); first_byte_res) {
  //     if (auto first_byte_opt = *first_byte_res; first_byte_opt) {
  //       const std::uint8_t first_byte = *first_byte_opt;
  //       if (auto length_opt = Utf8BytesLength(first_byte); length_opt) {
  //         const std::size_t length = *length_opt;
  //         std::uint8_t bytes[4] = {first_byte, 0, 0, 0};
  //         for (std::size_t i = 1; i < length; ++i) {
  //           if (auto cont_byte_res = ReadByte(*stream_); IsOk(cont_byte_res))
  //           {
  //             if (auto cont_byte_opt = Unwrap(cont_byte_res); cont_byte_opt)
  //             {
  //               const std::uint8_t cont_byte = *cont_byte_opt;
  //               if (IsUtf8ContinuationByte(cont_byte)) {
  //                 bytes[i] = cont_byte;
  //               } else {
  //                 return InvalidUtf8ContinuationByteError{
  //                     .position = position_ + i,
  //                     .continuation_byte = cont_byte};
  //               }
  //             } else {
  //               const ReadByteErrorKind kind = UnwrapErr(cont_byte_res);
  //             }
  //           } else {
  //             // error
  //           }
  //         }
  //       } else {
  //         return std::nullopt;
  //       }
  //     } else {
  //       const ReadByteErrorKind kind = UnwrapErr(first_byte_res);
  //       return ReadByteError{.kind = kind, .position = position_};
  //     }
  //   }
}

}  // namespace graphql_cpp
