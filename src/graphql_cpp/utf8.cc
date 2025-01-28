#include "graphql_cpp/utf8.h"

#include <optional>

#include "graphql_cpp/error_code.h"
#include "graphql_cpp/stream_reader.h"

namespace graphql_cpp {

namespace utf8 {

namespace {

DecodeResult ReadContinuationByte(
    StreamReader& r, std::function<DecodeResult(Byte&&)> f) noexcept {
  return AndThen(r.Read(), [f = std::move(f)](auto&& v) -> DecodeResult {
    return Visit(
        std::move(v),
        [f = std::move(f)](Byte&& b) -> DecodeResult {
          if (IsContinuationByte(b.value)) {
            return f(std::move(b));
          } else {
            return Error::Builder()
                .Code(ErrorCode::kUtf8InvalidContinuationByte)
                .Message("Invalid UTF-8 continuation byte.")
                .Detail("position", b.position)
                .Detail("byte", std::format("0x{:x}", b.value))
                .Build();
          }
        },
        [](Eof&& eof) -> DecodeResult {
          return Error::Builder()
              .Code(ErrorCode::kUtf8IncompleteBytes)
              .Message("Incomplete UTF-8 bytes.")
              .Detail("position", eof.position)
              .Build();
        });
  });
}

}  // namespace

DecodeResult Decode(StreamReader& r) noexcept {
  return AndThen(r.Read(), [&r](StreamReader::Item&& v) -> DecodeResult {
    return Visit(
        std::move(v),
        [&r](Byte&& b) -> DecodeResult { return DecodeFromFirstByte(r, b); },
        [](Eof&&) -> DecodeResult { return std::nullopt; });
  });
}

DecodeResult DecodeFromFirstByte(StreamReader& r, Byte b0) noexcept {
  if (IsAsciiCodePoint(b0.value)) {
    return CodePointFrom1Byte(b0.value);
  } else if (IsStartOf2ByteCodePoint(b0.value)) {
    return ReadContinuationByte(r, [b0](Byte&& b1) -> DecodeResult {
      return CodePointFrom2Bytes(b0.value, b1.value);
    });
  } else if (IsStartOf3ByteCodePoint(b0.value)) {
    return ReadContinuationByte(r, [&r, b0](Byte&& b1) -> DecodeResult {
      if (Is3ByteCodePointOverlong(b0.value, b1.value)) {
        return Error::Builder()
            .Code(ErrorCode::kUtf8OverlongOf3Bytes)
            .Message("UTF-8 3-byte code point overlong.")
            .Detail("position", b0.position)
            .Detail("byte0", std::format("0x{:x}", b0.value))
            .Detail("byte1", std::format("0x{:x}", b1.value))
            .Build();
      } else if (IsUnicodeSurrogate(b0.value, b1.value)) {
        return Error::Builder()
            .Code(ErrorCode::kUtf8UnicodeSurrogate)
            .Message("UTF-8 Unicode surrogate.")
            .Detail("position", b0.position)
            .Detail("byte0", std::format("0x{:x}", b0.value))
            .Detail("byte1", std::format("0x{:x}", b1.value))
            .Build();
      } else {
        return ReadContinuationByte(r, [b0, b1](Byte&& b2) -> DecodeResult {
          return CodePointFrom3Bytes(b0.value, b1.value, b2.value);
        });
      }
    });
  } else if (IsStartOf4ByteCodePoint(b0.value)) {
    return ReadContinuationByte(r, [&r, b0](Byte&& b1) -> DecodeResult {
      if (Is4ByteCodePointOverlong(b0.value, b1.value)) {
        return Error::Builder()
            .Code(ErrorCode::kUtf8OverlongOf4Bytes)
            .Message("UTF-8 4-byte code point overlong.")
            .Detail("position", b0.position)
            .Detail("byte0", std::format("0x{:x}", b0.value))
            .Detail("byte1", std::format("0x{:x}", b1.value))
            .Build();
      } else if (IsBeyondUnicodeRange(b0.value, b1.value)) {
        return Error::Builder()
            .Code(ErrorCode::kUtf8BeyondUnicodeRange)
            .Message("UTF-8 code point beyond Unicode range.")
            .Detail("position", b0.position)
            .Detail("byte0", std::format("0x{:x}", b0.value))
            .Detail("byte1", std::format("0x{:x}", b1.value))
            .Build();
      } else {
        return ReadContinuationByte(r, [&r, b0, b1](Byte&& b2) -> DecodeResult {
          return ReadContinuationByte(
              r, [b0, b1, b2](Byte&& b3) -> DecodeResult {
                return CodePointFrom4Bytes(b0.value, b1.value, b2.value,
                                           b3.value);
              });
        });
      }
    });
  } else {
    return Error::Builder()
        .Code(ErrorCode::kUtf8InvalidFirstByte)
        .Message("Invalid UTF-8 first byte.")
        .Detail("position", b0.position)
        .Detail("byte", std::format("0x{:x}", b0.value))
        .Build();
  }
}

bool IsAsciiCodePoint(std::uint8_t b) noexcept {
  // 0xxxxxxx, 0x00 ~ 0x7f
  return (b & 0b10000000) == 0b00000000;
}

bool IsStartOf2ByteCodePoint(std::uint8_t b) noexcept {
  // 110xxxxx, 0xc2 ~ 0xdf
  return (b & 0b11100000) == 0b11000000 && 0xc2 <= b;
}

bool IsStartOf3ByteCodePoint(std::uint8_t b) noexcept {
  // 1110xxxx, 0xe0 ~ 0xef
  return (b & 0b11110000) == 0b11100000;
}

bool IsStartOf4ByteCodePoint(std::uint8_t b) noexcept {
  // 11110xxx, 0xf0 ~ 0xf4
  return (b & 0b11111000) == 0b11110000 && b <= 0xf4;
}

bool IsContinuationByte(std::uint8_t b) noexcept {
  // 10xxxxxx
  return (b & 0b11000000) == 0b10000000;
}

// when 3-byte
bool Is3ByteCodePointOverlong(std::uint8_t b0, std::uint8_t b1) noexcept {
  return b0 == 0xe0 && b1 < 0xa0;
}

// when 3-byte
bool IsUnicodeSurrogate(std::uint8_t b0, std::uint8_t b1) noexcept {
  return b0 == 0xed && 0xa0 <= b1;
}

// when 4-byte
bool Is4ByteCodePointOverlong(std::uint8_t b0, std::uint8_t b1) noexcept {
  return b0 == 0xf0 && b1 < 0x90;
}

// when 4-byte
bool IsBeyondUnicodeRange(std::uint8_t b0, std::uint8_t b1) noexcept {
  return b0 == 0xf4 && 0x8f < b1;
}

std::uint32_t CodePointFrom1Byte(std::uint8_t b) noexcept { return b; }

std::uint32_t CodePointFrom2Bytes(std::uint8_t b0, std::uint8_t b1) noexcept {
  return (b0 & 0b00011111) << 6 | (b1 & 0b00111111);
}

std::uint32_t CodePointFrom3Bytes(std::uint8_t b0, std::uint8_t b1,
                                  std::uint8_t b2) noexcept {
  return (b0 & 0b00001111) << 12 | (b1 & 0b00111111) << 6 | (b2 & 0b00111111);
}

std::uint32_t CodePointFrom4Bytes(std::uint8_t b0, std::uint8_t b1,
                                  std::uint8_t b2, std::uint8_t b3) noexcept {
  return (b0 & 0b00000111) << 18 | (b1 & 0b00111111) << 12 |
         (b2 & 0b00111111) << 6 | (b3 & 0b00111111);
}

}  // namespace utf8

}  // namespace graphql_cpp
