#include "graphql_cpp/utf8_stream_reader.h"

#include <gtest/gtest.h>

#include "graphql_cpp/any_error.h"
#include "graphql_cpp/result.h"

namespace graphql_cpp {

TEST(utf8_stream_reader_string_stream, empty_stream) {
  std::istringstream stream;
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_ok(result));
  EXPECT_FALSE(unwrap(result));
}

TEST(utf8_stream_reader_string_stream, bad_stream) {
  std::istringstream stream;
  stream.setstate(std::ios::badbit);
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1 | 4);
  EXPECT_EQ(error.message, "A fatal error occurred while reading stream");
}

TEST(utf8_stream_reader_string_stream, fail_stream) {
  std::istringstream stream;
  stream.setstate(std::ios::failbit);
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 4);
  EXPECT_EQ(error.message, "A failure occurred while reading stream");
}

TEST(utf8_stream_reader_string_stream, invalid_utf8_first_byte) {
  std::istringstream stream("\x80");  // 0x80 is an invalid UTF-8 first byte
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "Invalid UTF-8 first byte: 0x80 at position: 0");
}

TEST(utf8_stream_reader_string_stream, invalid_utf8_continuation_byte) {
  std::istringstream stream("\xc2\x7f");  // 0xc2 is first byte of a 2-byte
                                          // code point, 0x7f is an invalid
                                          // UTF-8 continuation byte
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message,
            "Invalid UTF-8 continuation byte: 0x7f at position: 0");
}

TEST(utf8_stream_reader_string_stream, ascii_length_1) {
  std::istringstream stream("a");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'a');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, ascii_length_2) {
  std::istringstream stream("ab");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'a');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'b');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, ascii_range) {
  std::stringstream stream;
  for (std::uint8_t i = 0x00; i <= 0x7f; ++i) {
    stream.put(static_cast<char>(i));
  }
  Utf8StreamReader reader(stream);
  for (std::uint8_t i = 0x00; i <= 0x7f; ++i) {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), i);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, latin_small_letter_e_acute) {
  std::istringstream stream("é");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xe9);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream,
     latin_small_letter_e_acute_latin_small_letter_n_tilde) {
  std::istringstream stream("éñ");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xe9);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xf1);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream,
     latin_small_letter_a_latin_small_letter_e_acute) {
  std::istringstream stream("aé");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'a');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xe9);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream,
     latin_small_letter_e_acute_latin_small_letter_a) {
  std::istringstream stream("éa");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xe9);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'a');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, cyrillic) {
  std::istringstream stream("б");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x0431);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, ascii_cyrillic) {
  std::istringstream stream("zб");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'z');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x0431);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, cyrillic_ascii) {
  std::istringstream stream("бz");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x0431);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'z');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, arabic) {
  std::istringstream stream("ب");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x0628);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, korean) {
  std::istringstream stream("가");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xac00);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, korean_ascii) {
  std::istringstream stream("가z");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xac00);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'z');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, ascii_korean) {
  std::istringstream stream("z가");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'z');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0xac00);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, emoji) {
  std::istringstream stream("👍");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x1f44d);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, emoji_ascii) {
  std::istringstream stream("👍z");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x1f44d);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'z');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, ascii_emoji) {
  std::istringstream stream("z👍");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 'z');
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x1f44d);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, overlong_encoding) {
  // 'A' (U+0041) encoded as 3 bytes instead of 1
  std::istringstream stream("\xe0\x81\x81");
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "Invalid overlong UTF-8 encoding at position: 0");
}

TEST(utf8_stream_reader_string_stream, incomplete_sequence) {
  // Incomplete 3-byte sequence
  std::istringstream stream("\xe0\x81");
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "Incomplete UTF-8 sequence at position: 0");
}

TEST(utf8_stream_reader_string_stream, surrogate_pair) {
  // UTF-16 surrogate pair (U+D800-U+DFFF) - invalid in UTF-8
  std::istringstream stream("\xed\xa0\x80");
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message,
            "Invalid UTF-8 surrogate pair encoding at position: 0");
}

TEST(utf8_stream_reader_string_stream, max_valid_codepoint) {
  // U+10FFFF - Maximum valid Unicode code point
  std::istringstream stream("\xf4\x8f\xbf\xbf");
  Utf8StreamReader reader(stream);
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_EQ(unwrap(result), 0x10ffff);
  }
  {
    auto result = reader.read();
    EXPECT_TRUE(is_ok(result));
    EXPECT_FALSE(unwrap(result));
  }
}

TEST(utf8_stream_reader_string_stream, beyond_unicode_range) {
  // Beyond U+10FFFF
  std::istringstream stream("\xf4\x90\x80\x80");
  Utf8StreamReader reader(stream);
  auto result = reader.read();
  EXPECT_TRUE(is_err(result));
  auto&& error = unwrap_err(std::move(result));
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "Code point beyond Unicode range at position: 0");
}

}  // namespace graphql_cpp
