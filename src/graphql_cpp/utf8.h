#ifndef GRAPHQL_CPP_UTF8_H_
#define GRAPHQL_CPP_UTF8_H_

#include <cstdint>

#include "graphql_cpp/result.h"
#include "graphql_cpp/stream_reader.h"

namespace graphql_cpp {

namespace utf8 {

using CodePoint = std::uint32_t;

using DecodeResult = Result<std::optional<CodePoint>, Error>;

DecodeResult Decode(StreamReader& reader) noexcept;
DecodeResult DecodeFromFirstByte(StreamReader& reader,
                                 Byte first_byte) noexcept;

bool IsAsciiCodePoint(std::uint8_t byte) noexcept;
bool IsStartOf2ByteCodePoint(std::uint8_t byte) noexcept;
bool IsStartOf3ByteCodePoint(std::uint8_t byte) noexcept;
bool IsStartOf4ByteCodePoint(std::uint8_t byte) noexcept;
bool IsContinuationByte(std::uint8_t byte) noexcept;
bool Is3ByteCodePointOverlong(std::uint8_t b0, std::uint8_t b1) noexcept;
bool IsUnicodeSurrogate(std::uint8_t b0, std::uint8_t b1) noexcept;
bool Is4ByteCodePointOverlong(std::uint8_t b0, std::uint8_t b1) noexcept;
bool IsBeyondUnicodeRange(std::uint8_t b0, std::uint8_t b1) noexcept;

std::uint32_t CodePointFrom1Byte(std::uint8_t b) noexcept;
std::uint32_t CodePointFrom2Bytes(std::uint8_t b0, std::uint8_t b1) noexcept;
std::uint32_t CodePointFrom3Bytes(std::uint8_t b0, std::uint8_t b1,
                                  std::uint8_t b2) noexcept;
std::uint32_t CodePointFrom4Bytes(std::uint8_t b0, std::uint8_t b1,
                                  std::uint8_t b2, std::uint8_t b3) noexcept;

}  // namespace utf8

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_H_
