#include "graphql_cpp/utf8.h"

#include <gtest/gtest.h>

#include "graphql_cpp/error_code.h"
#include "graphql_cpp/result.h"

namespace graphql_cpp {

namespace utf8 {

TEST(Utf8DecodeReaderStringStream, InvalidFirstByte) {
  for (std::uint8_t i = 0x80; i <= 0xc1; ++i) {
    std::istringstream stream(std::string(1, static_cast<char>(i)));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    EXPECT_TRUE(IsErr(result));
    auto&& error = UnwrapErr(std::move(result));
    EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8InvalidFirstByte));
    EXPECT_EQ(error.Message(), "Invalid UTF-8 first byte.");
    EXPECT_EQ(error.Detail("position").value(), "0");
    EXPECT_EQ(error.Detail("byte").value(), std::format("0x{:x}", i));
  }
  for (std::uint8_t i = 0xf5; 0xf5 <= i && i <= 0xff; ++i) {
    std::istringstream stream(std::string(1, static_cast<char>(i)));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    EXPECT_TRUE(IsErr(result));
    auto&& error = UnwrapErr(std::move(result));
    EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8InvalidFirstByte));
    EXPECT_EQ(error.Message(), "Invalid UTF-8 first byte.");
    EXPECT_EQ(error.Detail("position").value(), "0");
    EXPECT_EQ(error.Detail("byte").value(), std::format("0x{:x}", i));
  }
}

TEST(Utf8DecodeReaderStringStream, IncompleteUtf8Bytes) {
  for (std::uint8_t i = 0xc2; i <= 0xf4; ++i) {
    std::istringstream stream(std::string(1, static_cast<char>(i)));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    EXPECT_TRUE(IsErr(result));
    auto&& error = UnwrapErr(std::move(result));
    EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8IncompleteBytes));
    EXPECT_EQ(error.Message(), "Incomplete UTF-8 bytes.");
    EXPECT_EQ(error.Detail("position").value(), "1");
  }
}

TEST(Utf8DecodeReaderStringStream, InvalidUtf8ContinuationByte) {
  for (std::uint8_t i = 0x00; i <= 0x7f; ++i) {
    std::stringstream stream;
    stream.put(static_cast<char>(0xc2));
    stream.put(static_cast<char>(i));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    EXPECT_TRUE(IsErr(result));
    auto&& error = UnwrapErr(std::move(result));
    EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8InvalidContinuationByte));
    EXPECT_EQ(error.Message(), "Invalid UTF-8 continuation byte.");
    EXPECT_EQ(error.Detail("position").value(), "1");
    EXPECT_EQ(error.Detail("byte").value(), std::format("0x{:x}", i));
  }
  for (std::uint8_t i = 0x80; i <= 0xbf; ++i) {
    std::stringstream stream;
    stream.put(static_cast<char>(0xc2));
    stream.put(static_cast<char>(i));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    EXPECT_TRUE(IsOk(result));
    auto&& code_point = Unwrap(std::move(result));
    EXPECT_TRUE(code_point);
    auto&& eof_result = Decode(reader);
    EXPECT_TRUE(IsOk(eof_result));
    EXPECT_FALSE(Unwrap(std::move(eof_result)));
  }
  for (std::uint8_t i = 0xc0; 0xc0 <= i && i <= 0xff; ++i) {
    std::stringstream stream;
    stream.put(static_cast<char>(0xc2));
    stream.put(static_cast<char>(i));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    EXPECT_TRUE(IsErr(result));
    auto&& error = UnwrapErr(std::move(result));
    EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8InvalidContinuationByte));
    EXPECT_EQ(error.Message(), "Invalid UTF-8 continuation byte.");
    EXPECT_EQ(error.Detail("position").value(), "1");
    EXPECT_EQ(error.Detail("byte").value(), std::format("0x{:x}", i));
  }
}

TEST(Utf8DecodeReaderStringStream, Bytes3CodePointOverlong) {
  std::stringstream stream;
  stream.put(static_cast<char>(0xe0));
  stream.put(static_cast<char>(0x8f));
  stream.put(static_cast<char>(0x80));
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsErr(result));
  auto&& error = UnwrapErr(std::move(result));
  EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8OverlongOf3Bytes));
  EXPECT_EQ(error.Message(), "UTF-8 3-byte code point overlong.");
  EXPECT_EQ(error.Detail("position").value(), "0");
}

TEST(Utf8DecodeReaderStringStream, UnicodeSurrogate) {
  std::stringstream stream;
  stream.put(static_cast<char>(0xed));
  stream.put(static_cast<char>(0xa0));
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsErr(result));
  auto&& error = UnwrapErr(std::move(result));
  EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8UnicodeSurrogate));
  EXPECT_EQ(error.Message(), "UTF-8 Unicode surrogate.");
  EXPECT_EQ(error.Detail("position").value(), "0");
}

TEST(Utf8DecodeReaderStringStream, Bytes4CodePointOverlong) {
  std::stringstream stream;
  stream.put(static_cast<char>(0xf0));
  stream.put(static_cast<char>(0x8f));
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsErr(result));
  auto&& error = UnwrapErr(std::move(result));
  EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8OverlongOf4Bytes));
  EXPECT_EQ(error.Message(), "UTF-8 4-byte code point overlong.");
  EXPECT_EQ(error.Detail("position").value(), "0");
}

TEST(Utf8DecodeReaderStringStream, BeyondUnicodeRange) {
  std::stringstream stream;
  stream.put(static_cast<char>(0xf4));
  stream.put(static_cast<char>(0x90));
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsErr(result));
  auto&& error = UnwrapErr(std::move(result));
  EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kUtf8BeyondUnicodeRange));
  EXPECT_EQ(error.Message(), "UTF-8 code point beyond Unicode range.");
  EXPECT_EQ(error.Detail("position").value(), "0");
}

TEST(Utf8DecodeReaderStringStream, AsciiRange) {
  for (std::uint8_t i = 0x00; i <= 0x7f; ++i) {
    std::istringstream stream(std::string(1, static_cast<char>(i)));
    StreamReader reader(stream);
    auto&& result = Decode(reader);
    auto&& code_point = Unwrap(std::move(result));
    EXPECT_TRUE(code_point);
    auto&& eof_result = Decode(reader);
    EXPECT_TRUE(IsOk(eof_result));
    EXPECT_FALSE(Unwrap(std::move(eof_result)));
  }
}

TEST(Utf8DecodeReaderStringStream, Bytes2CodePoint) {
  for (std::uint8_t i = 0xc2; i <= 0xdf; ++i) {
    for (std::uint8_t j = 0x80; j <= 0xbf; ++j) {
      std::stringstream stream;
      stream.put(static_cast<char>(i));
      stream.put(static_cast<char>(j));
      StreamReader reader(stream);
      auto&& result = Decode(reader);
      EXPECT_TRUE(IsOk(result));
      auto&& code_point = Unwrap(std::move(result));
      EXPECT_TRUE(code_point);
      auto&& eof_result = Decode(reader);
      EXPECT_TRUE(IsOk(eof_result));
      EXPECT_FALSE(Unwrap(std::move(eof_result)));
    }
  }
}

TEST(Utf8DecodeReaderStringStream, Bytes3CodePoint) {
  for (std::uint8_t i = 0xe0; i <= 0xef; ++i) {
    for (std::uint8_t j = 0x80; j <= 0xbf; ++j) {
      if (i == 0xe0 && j < 0xa0) {
        // 3-byte code point overlong
        continue;
      }
      if (i == 0xed && 0xa0 <= j) {
        // Unicode surrogate
        continue;
      }
      for (std::uint8_t k = 0x80; k <= 0xbf; ++k) {
        std::stringstream stream;
        stream.put(static_cast<char>(i));
        stream.put(static_cast<char>(j));
        stream.put(static_cast<char>(k));
        StreamReader reader(stream);
        auto&& result = Decode(reader);
        auto&& code_point = Unwrap(std::move(result));
        EXPECT_TRUE(code_point);
        auto&& eof_result = Decode(reader);
        EXPECT_TRUE(IsOk(eof_result));
        EXPECT_FALSE(Unwrap(std::move(eof_result)));
      }
    }
  }
}

TEST(Utf8DecodeReaderStringStream, Bytes4CodePoint) {
  for (std::uint8_t i = 0xf0; i <= 0xf4; ++i) {
    for (std::uint8_t j = 0x80; j <= 0xbf; ++j) {
      if (i == 0xf0 && j < 0x90) {
        // 4-byte code point overlong
        continue;
      }
      if (i == 0xf4 && 0x8f < j) {
        // Beyond Unicode range
        continue;
      }
      for (std::uint8_t k = 0x80; k <= 0xbf; ++k) {
        for (std::uint8_t l = 0x80; l <= 0xbf; ++l) {
          std::stringstream stream;
          stream.put(static_cast<char>(i));
          stream.put(static_cast<char>(j));
          stream.put(static_cast<char>(k));
          stream.put(static_cast<char>(l));
          StreamReader reader(stream);
          auto&& result = Decode(reader);
          auto&& code_point = Unwrap(std::move(result));
          EXPECT_TRUE(code_point);
          auto&& eof_result = Decode(reader);
          EXPECT_TRUE(IsOk(eof_result));
          EXPECT_FALSE(Unwrap(std::move(eof_result)));
        }
      }
    }
  }
}

TEST(Utf8DecodeReaderStringStream, Bytes2CodePointExample) {
  std::stringstream stream("¡");
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsOk(result));
  auto&& code_point = Unwrap(std::move(result));
  EXPECT_TRUE(code_point);
  EXPECT_EQ(*code_point, 0x00a1);
  auto&& eof_result = Decode(reader);
  EXPECT_TRUE(IsOk(eof_result));
  EXPECT_FALSE(Unwrap(std::move(eof_result)));
}

TEST(Utf8DecodeReaderStringStream, Bytes3CodePointExample) {
  std::stringstream stream("가");
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsOk(result));
  auto&& code_point = Unwrap(std::move(result));
  EXPECT_TRUE(code_point);
  EXPECT_EQ(*code_point, 0xac00);
  auto&& eof_result = Decode(reader);
  EXPECT_TRUE(IsOk(eof_result));
  EXPECT_FALSE(Unwrap(std::move(eof_result)));
}

TEST(Utf8DecodeReaderStringStream, Bytes4CodePointExample) {
  std::stringstream stream("🌍");
  StreamReader reader(stream);
  auto&& result = Decode(reader);
  EXPECT_TRUE(IsOk(result));
  auto&& code_point = Unwrap(std::move(result));
  EXPECT_TRUE(code_point);
  EXPECT_EQ(*code_point, 0x1f30d);
  auto&& eof_result = Decode(reader);
  EXPECT_TRUE(IsOk(eof_result));
  EXPECT_FALSE(Unwrap(std::move(eof_result)));
}

}  // namespace utf8

}  // namespace graphql_cpp
