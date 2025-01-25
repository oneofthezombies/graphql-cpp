#include "graphql_cpp/utf8_stream.h"

#include <gtest/gtest.h>

#include <fstream>

namespace graphql_cpp {

TEST(Utf8StreamTest, StringStreamEmpty) {
  Utf8Stream stream(std::make_unique<std::istringstream>(""));
  auto res = stream.Next();
  EXPECT_TRUE(IsOk(res));
  auto code_point_opt = Unwrap(res);
  EXPECT_FALSE(code_point_opt);
}

TEST(Utf8StreamTest, StringStreamCodePoint) {
  Utf8Stream stream(std::make_unique<std::stringstream>("A"));
  {
    auto res = stream.Next();
    EXPECT_TRUE(IsOk(res));
    auto code_point_opt = Unwrap(res);
    EXPECT_TRUE(code_point_opt);
    EXPECT_EQ(*code_point_opt, 0x41);
  }
  {
    auto res = stream.Next();
    EXPECT_TRUE(IsOk(res));
    auto code_point_opt = Unwrap(res);
    EXPECT_FALSE(code_point_opt);
  }
}

TEST(Utf8StreamTest, FileStreamEmpty) {
  { std::ofstream ofs("empty_file.txt"); }
  Utf8Stream stream(std::make_unique<std::ifstream>("empty_file.txt"));
  auto res = stream.Next();
  EXPECT_TRUE(IsOk(res));
  auto code_point_opt = Unwrap(res);
  EXPECT_FALSE(code_point_opt);
}

TEST(Utf8StreamTest, FileStreamCodePoint) {
  {
    std::ofstream ofs("code_point_file.txt");
    ofs << "A";
  }
  Utf8Stream stream(std::make_unique<std::ifstream>("code_point_file.txt"));
  {
    auto res = stream.Next();
    EXPECT_TRUE(IsOk(res));
    auto code_point_opt = Unwrap(res);
    EXPECT_TRUE(code_point_opt);
    EXPECT_EQ(*code_point_opt, 0x41);
  }
  {
    auto res = stream.Next();
    EXPECT_TRUE(IsOk(res));
    auto code_point_opt = Unwrap(res);
    EXPECT_FALSE(code_point_opt);
  }
}

TEST(Utf8StreamTest, Utf8FirstByteError) {
  // invalid first byte
  Utf8Stream stream(std::make_unique<std::stringstream>("\x80"));
  auto res = stream.Next();
  EXPECT_TRUE(IsErr(res));
  auto error = UnwrapErr(res);
  EXPECT_TRUE(std::holds_alternative<Utf8FirstByteError>(error));
  auto first_byte_error = std::get<Utf8FirstByteError>(error);
  EXPECT_EQ(first_byte_error.position, 0);
  EXPECT_EQ(first_byte_error.first_byte, 0x80);
}

TEST(Utf8StreamTest, Utf8CodePointLengthError) {
  // first byte of a 2-byte code point
  Utf8Stream stream(std::make_unique<std::stringstream>("\xC2"));
  auto res = stream.Next();
  EXPECT_TRUE(IsErr(res));
  auto error = UnwrapErr(res);
  EXPECT_TRUE(std::holds_alternative<Utf8CodePointLengthError>(error));
  auto code_point_length_error = std::get<Utf8CodePointLengthError>(error);
  EXPECT_EQ(code_point_length_error.position, 0);
  EXPECT_EQ(code_point_length_error.code_point_length, 2);
}

TEST(Utf8StreamTest, Utf8ContinuationByteError) {
  // first byte of a 2-byte code point but the second byte is not a continuation
  // byte
  Utf8Stream stream(std::make_unique<std::stringstream>("\xC2\x41"));
  auto res = stream.Next();
  EXPECT_TRUE(IsErr(res));
  auto error = UnwrapErr(res);
  EXPECT_TRUE(std::holds_alternative<Utf8ContinuationByteError>(error));
  auto continuation_byte_error = std::get<Utf8ContinuationByteError>(error);
  EXPECT_EQ(continuation_byte_error.position, 1);
  EXPECT_EQ(continuation_byte_error.continuation_byte, 0x41);
}

}  // namespace graphql_cpp
