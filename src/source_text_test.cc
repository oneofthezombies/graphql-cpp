#include "source_text.h"

#include <gtest/gtest.h>

namespace graphql_cpp {

TEST(SourceTextTest, EmptyText) {
  std::uint8_t text[] = {};
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 0);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  source_text::SourceText source_text(text_slice);
  auto res = source_text.NextSourceCharacter();
  EXPECT_TRUE(std::holds_alternative<source_text::Eof>(res));
}

TEST(SourceTextTest, SourceCharacter) {
  std::uint8_t text[] = {0x41};  // 'A'
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  source_text::SourceText source_text(text_slice);
  auto res = source_text.NextSourceCharacter();
  EXPECT_TRUE(std::holds_alternative<source_text::SourceCharacter>(res));
  auto source_character = std::get<source_text::SourceCharacter>(res);
  EXPECT_EQ(source_character.code_point, 0x41);
}

TEST(SourceTextTest, NonSourceCharacter) {
  std::uint8_t text[] = {0x19};  // '<End of Medium>'
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  source_text::SourceText source_text(text_slice);
  auto res = source_text.NextSourceCharacter();
  EXPECT_TRUE(std::holds_alternative<source_text::NonSourceCharacter>(res));
  auto non_source_character = std::get<source_text::NonSourceCharacter>(res);
  EXPECT_EQ(non_source_character.code_point, 0x19);
}

TEST(SourceTextTest, FirstByteError) {
  std::uint8_t text[] = {0x80};  // Invalid first byte
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  source_text::SourceText source_text(text_slice);
  auto res = source_text.NextSourceCharacter();
  EXPECT_TRUE(std::holds_alternative<source_text::FirstByteError>(res));
  auto first_byte_error = std::get<source_text::FirstByteError>(res);
  EXPECT_EQ(first_byte_error.position, 0);
  EXPECT_EQ(first_byte_error.first_byte, 0x80);
}

TEST(SourceTextTest, CodePointLengthError) {
  std::uint8_t text[] = {0xC2};  // first byte of a 2-byte code point
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 1);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  source_text::SourceText source_text(text_slice);
  auto res = source_text.NextSourceCharacter();
  EXPECT_TRUE(std::holds_alternative<source_text::CodePointLengthError>(res));
  auto code_point_length_error =
      std::get<source_text::CodePointLengthError>(res);
  EXPECT_EQ(code_point_length_error.position, 0);
  EXPECT_EQ(code_point_length_error.code_point_length, 2);
  EXPECT_EQ(code_point_length_error.text_length, 1);
}

TEST(SourceTextTest, ContinuationByteError) {
  std::uint8_t text[] = {0xC2,
                         0x41};  // first byte of a 2-byte code point but the
                                 // second byte is not a continuation byte
  auto create_slice_res = slice::Slice<std::uint8_t>::Create(text, 2);
  EXPECT_TRUE(
      std::holds_alternative<slice::Slice<std::uint8_t>>(create_slice_res));
  auto text_slice = std::get<slice::Slice<std::uint8_t>>(create_slice_res);
  source_text::SourceText source_text(text_slice);
  auto res = source_text.NextSourceCharacter();
  EXPECT_TRUE(std::holds_alternative<source_text::ContinuationByteError>(res));
  auto continuation_byte_error =
      std::get<source_text::ContinuationByteError>(res);
  EXPECT_EQ(continuation_byte_error.position, 1);
  EXPECT_EQ(continuation_byte_error.continuation_byte, 0x41);
}

}  // namespace graphql_cpp
