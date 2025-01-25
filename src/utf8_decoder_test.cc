#include "utf8_decoder.h"

#include <gtest/gtest.h>

#include "slice.h"

namespace graphql_cpp {

TEST(Utf8DecoderTest, EmptyText) {
  std::uint8_t text[] = {};
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 0);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  utf8_decoder::Utf8Decoder decoder(text_slice);
  auto res = decoder.NextCodePoint();
  EXPECT_TRUE(std::holds_alternative<utf8_decoder::Eof>(res));
}

TEST(Utf8DecoderTest, CodePoint) {
  std::uint8_t text[] = {0x41};  // 'A'
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  utf8_decoder::Utf8Decoder decoder(text_slice);
  auto res = decoder.NextCodePoint();
  EXPECT_TRUE(std::holds_alternative<utf8_decoder::CodePoint>(res));
  auto code_point = std::get<utf8_decoder::CodePoint>(res);
  EXPECT_EQ(code_point.value, 0x41);
}

TEST(Utf8DecoderTest, FirstByteError) {
  std::uint8_t text[] = {0x80};  // Invalid first byte
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  utf8_decoder::Utf8Decoder decoder(text_slice);
  auto res = decoder.NextCodePoint();
  EXPECT_TRUE(std::holds_alternative<utf8_decoder::FirstByteError>(res));
  auto first_byte_error = std::get<utf8_decoder::FirstByteError>(res);
  EXPECT_EQ(first_byte_error.position, 0);
  EXPECT_EQ(first_byte_error.first_byte, 0x80);
}

TEST(Utf8DecoderTest, CodePointLengthError) {
  std::uint8_t text[] = {0xC2};  // first byte of a 2-byte code point
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  utf8_decoder::Utf8Decoder decoder(text_slice);
  auto res = decoder.NextCodePoint();
  EXPECT_TRUE(std::holds_alternative<utf8_decoder::CodePointLengthError>(res));
  auto code_point_length_error =
      std::get<utf8_decoder::CodePointLengthError>(res);
  EXPECT_EQ(code_point_length_error.position, 0);
  EXPECT_EQ(code_point_length_error.code_point_length, 2);
  EXPECT_EQ(code_point_length_error.text_length, 1);
}

TEST(Utf8DecoderTest, ContinuationByteError) {
  std::uint8_t text[] = {0xC2,
                         0x41};  // first byte of a 2-byte code point but the
                                 // second byte is not a continuation byte
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 2);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  utf8_decoder::Utf8Decoder decoder(text_slice);
  auto res = decoder.NextCodePoint();
  EXPECT_TRUE(std::holds_alternative<utf8_decoder::ContinuationByteError>(res));
  auto continuation_byte_error =
      std::get<utf8_decoder::ContinuationByteError>(res);
  EXPECT_EQ(continuation_byte_error.position, 1);
  EXPECT_EQ(continuation_byte_error.continuation_byte, 0x41);
}

}  // namespace graphql_cpp
