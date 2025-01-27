#include "graphql_cpp/utf8_decode_reader.h"

#include <format>

namespace graphql_cpp {

constexpr std::optional<std::size_t> Utf8BytesLength(std::uint8_t b) noexcept {
  if ((b & 0b10000000) == 0b00000000) {
    // 0xxxxxxx, 0x00 ~ 0x7f
    return 1;
  } else if ((b & 0b11100000) == 0b11000000 && 0xc2 <= b) {
    // 110xxxxx, 0xc2 ~ 0xdf
    return 2;
  } else if ((b & 0b11110000) == 0b11100000) {
    // 1110xxxx, 0xe0 ~ 0xef
    return 3;
  } else if ((b & 0b11111000) == 0b11110000 && b <= 0xf4) {
    // 11110xxx, 0xf0 ~ 0xf4
    return 4;
  } else {
    return std::nullopt;
  }
}

constexpr bool IsUtf8ContinuationByte(std::uint8_t b) noexcept {
  // 10xxxxxx
  return (b & 0b11000000) == 0b10000000;
}

// when 3-byte
constexpr bool Is3ByteCodePointOverlong(std::uint8_t b0,
                                        std::uint8_t b1) noexcept {
  return b0 == 0xe0 && b1 < 0xa0;
}

// when 3-byte
constexpr bool IsUnicodeSurrogate(std::uint8_t b0, std::uint8_t b1) noexcept {
  return b0 == 0xed && 0xa0 <= b1;
}

// when 4-byte
constexpr bool Is4ByteCodePointOverlong(std::uint8_t b0,
                                        std::uint8_t b1) noexcept {
  return b0 == 0xf0 && b1 < 0x90;
}

// when 4-byte
constexpr bool IsBeyondUnicodeRange(std::uint8_t b0, std::uint8_t b1) noexcept {
  return b0 == 0xf4 && 0x8f < b1;
}

constexpr std::uint32_t Utf8CodePointFrom1Byte(std::uint8_t b) noexcept {
  return b;
}

constexpr std::uint32_t Utf8CodePointFrom2Bytes(std::uint8_t b0,
                                                std::uint8_t b1) noexcept {
  return (b0 & 0b00011111) << 6 | (b1 & 0b00111111);
}

constexpr std::uint32_t Utf8CodePointFrom3Bytes(std::uint8_t b0,
                                                std::uint8_t b1,
                                                std::uint8_t b2) noexcept {
  return (b0 & 0b00001111) << 12 | (b1 & 0b00111111) << 6 | (b2 & 0b00111111);
}

constexpr std::uint32_t Utf8CodePointFrom4Bytes(std::uint8_t b0,
                                                std::uint8_t b1,
                                                std::uint8_t b2,
                                                std::uint8_t b3) noexcept {
  return (b0 & 0b00000111) << 18 | (b1 & 0b00111111) << 12 |
         (b2 & 0b00111111) << 6 | (b3 & 0b00111111);
}

Utf8DecodeReader::Error MakeInvalidUtf8FirstByteError(
    Utf8DecodeReader::Byte b0) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kInvalidUtf8FirstByte)
      .Message("Invalid UTF-8 first byte.")
      .Detail("position", b0.position)
      .Detail("byte", std::format("0x{:x}", b0.value))
      .Build();
}

Utf8DecodeReader::Error MakeBadStreamStateError() noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kBadStreamState)
      .Message("Bad stream state.")
      .Build();
}

Utf8DecodeReader::Error MakeFailStreamStateError() noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kFailStreamState)
      .Message("Fail stream state.")
      .Build();
}

Utf8DecodeReader::Error MakeUnexpectedStreamStateError(
    std::size_t position) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kUnexpectedStreamState)
      .Message("Unexpected stream state.")
      .Detail("position", position)
      .Build();
}

Utf8DecodeReader::Error MakeIncompleteUtf8BytesError(
    Utf8DecodeReader::Eof eof) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kIncompleteUtf8Bytes)
      .Message("Incomplete UTF-8 bytes.")
      .Detail("position", eof.position)
      .Build();
}

Utf8DecodeReader::Error MakeInvalidUtf8ContinuationByteError(
    Utf8DecodeReader::Byte byte) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kInvalidUtf8ContinuationByte)
      .Message("Invalid UTF-8 continuation byte.")
      .Detail("position", byte.position)
      .Detail("byte", std::format("0x{:x}", byte.value))
      .Build();
}

Utf8DecodeReader::Error Make3ByteCodePointOverlongError(
    Utf8DecodeReader::Byte b0, Utf8DecodeReader::Byte b1) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::k3ByteCodePointOverlong)
      .Message("3-byte code point overlong.")
      .Detail("position", b0.position)
      .Detail("byte0", std::format("0x{:x}", b0.value))
      .Detail("byte1", std::format("0x{:x}", b1.value))
      .Build();
}

Utf8DecodeReader::Error MakeUnicodeSurrogateError(
    Utf8DecodeReader::Byte b0, Utf8DecodeReader::Byte b1) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kUnicodeSurrogate)
      .Message("Unicode surrogate.")
      .Detail("position", b0.position)
      .Detail("byte0", std::format("0x{:x}", b0.value))
      .Detail("byte1", std::format("0x{:x}", b1.value))
      .Build();
}

Utf8DecodeReader::Error Make4ByteCodePointOverlongError(
    Utf8DecodeReader::Byte b0, Utf8DecodeReader::Byte b1) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::k4ByteCodePointOverlong)
      .Message("4-byte code point overlong.")
      .Detail("position", b0.position)
      .Detail("byte0", std::format("0x{:x}", b0.value))
      .Detail("byte1", std::format("0x{:x}", b1.value))
      .Build();
}

Utf8DecodeReader::Error MakeBeyondUnicodeRangeError(
    Utf8DecodeReader::Byte b0, Utf8DecodeReader::Byte b1) noexcept {
  return Utf8DecodeReader::Error::Builder()
      .Code(Utf8DecodeReader::ErrorCode::kBeyondUnicodeRange)
      .Message("Beyond Unicode range.")
      .Detail("position", b0.position)
      .Detail("byte0", std::format("0x{:x}", b0.value))
      .Detail("byte1", std::format("0x{:x}", b1.value))
      .Build();
}

Utf8DecodeReader::Utf8DecodeReader(std::istream& stream) noexcept
    : stream_(stream), stream_position_(stream_.tellg()) {}

Utf8DecodeReader::ReadResult Utf8DecodeReader::Read() noexcept {
  return AndThen(ReadByte(), [this](auto&& v) -> ReadResult {
    return Visit(
        std::move(v),
        [this](Byte b0) -> ReadResult {
          auto&& bytes_len = Utf8BytesLength(b0.value);
          if (bytes_len) {
            if (*bytes_len == 1) {
              return Utf8CodePointFrom1Byte(b0.value);
            } else {
              return ReadSecondByte(*bytes_len, b0);
            }
          } else {
            return MakeInvalidUtf8FirstByteError(b0);
          }
        },
        [](Eof eof) -> ReadResult { return std::nullopt; });
  });
}

Utf8DecodeReader::ReadByteResult Utf8DecodeReader::ReadByte() noexcept {
  const std::size_t pos = stream_position_;
  char c{};
  stream_.get(c);
  stream_position_++;
  if (stream_.good()) {
    return Byte{static_cast<std::uint8_t>(c), pos};
  } else if (stream_.eof()) {
    return Eof{pos};
  } else if (stream_.bad()) {
    return MakeBadStreamStateError();
  } else if (stream_.fail()) {
    return MakeFailStreamStateError();
  } else {
    return MakeUnexpectedStreamStateError(pos);
  }
}

Utf8DecodeReader::ReadResult Utf8DecodeReader::ReadSecondByte(
    std::size_t bytes_length, Byte b0) noexcept {
  return AndThen(ReadByte(), [this, bytes_length, b0](auto&& v) -> ReadResult {
    return Visit(
        std::move(v),
        [this, bytes_length, b0](Byte b1) -> ReadResult {
          if (IsUtf8ContinuationByte(b1.value)) {
            if (bytes_length == 2) {
              return Utf8CodePointFrom2Bytes(b0.value, b1.value);
            } else if (bytes_length == 3) {
              if (Is3ByteCodePointOverlong(b0.value, b1.value)) {
                return Make3ByteCodePointOverlongError(b0, b1);
              } else if (IsUnicodeSurrogate(b0.value, b1.value)) {
                return MakeUnicodeSurrogateError(b0, b1);
              }
            } else if (bytes_length == 4) {
              if (Is4ByteCodePointOverlong(b0.value, b1.value)) {
                return Make4ByteCodePointOverlongError(b0, b1);
              } else if (IsBeyondUnicodeRange(b0.value, b1.value)) {
                return MakeBeyondUnicodeRangeError(b0, b1);
              }
            }

            return ReadThirdByte(bytes_length, b0, b1);
          } else {
            return MakeInvalidUtf8ContinuationByteError(b1);
          }
        },
        [this](Eof eof) -> ReadResult {
          return MakeIncompleteUtf8BytesError(eof);
        });
  });
}

Utf8DecodeReader::ReadResult Utf8DecodeReader::ReadThirdByte(
    std::size_t bytes_length, Byte b0, Byte b1) noexcept {
  return AndThen(
      ReadByte(), [this, bytes_length, b0, b1](auto&& v) -> ReadResult {
        return Visit(
            std::move(v),
            [this, bytes_length, b0, b1](Byte b2) -> ReadResult {
              if (IsUtf8ContinuationByte(b2.value)) {
                if (bytes_length == 3) {
                  return Utf8CodePointFrom3Bytes(b0.value, b1.value, b2.value);
                } else {
                  return ReadFourthByte(b0, b1, b2);
                }
              } else {
                return MakeInvalidUtf8ContinuationByteError(b2);
              }
            },
            [this](Eof eof) -> ReadResult {
              return MakeIncompleteUtf8BytesError(eof);
            });
      });
}

Utf8DecodeReader::ReadResult Utf8DecodeReader::ReadFourthByte(
    Byte b0, Byte b1, Byte b2) noexcept {
  return AndThen(ReadByte(), [this, b0, b1, b2](auto&& v) -> ReadResult {
    return Visit(
        std::move(v),
        [this, b0, b1, b2](Byte b3) -> ReadResult {
          if (IsUtf8ContinuationByte(b3.value)) {
            return Utf8CodePointFrom4Bytes(b0.value, b1.value, b2.value,
                                           b3.value);
          } else {
            return MakeInvalidUtf8ContinuationByteError(b3);
          }
        },
        [this](Eof eof) -> ReadResult {
          return MakeIncompleteUtf8BytesError(eof);
        });
  });
}

}  // namespace graphql_cpp
