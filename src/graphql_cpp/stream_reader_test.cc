#include "graphql_cpp/stream_reader.h"

#include <gtest/gtest.h>

#include <sstream>

#include "graphql_cpp/error_code.h"

namespace graphql_cpp {

TEST(StreamReader, BadStream) {
  std::istringstream stream;
  stream.setstate(std::ios::badbit);
  StreamReader reader(stream);
  auto&& result = reader.Read();
  EXPECT_TRUE(IsErr(result));
  auto&& error = UnwrapErr(std::move(result));
  EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kStreamBad));
  EXPECT_EQ(error.Message(), "Stream state is bad.");
}

TEST(StreamReader, FailStream) {
  std::istringstream stream;
  stream.setstate(std::ios::failbit);
  StreamReader reader(stream);
  auto&& result = reader.Read();
  EXPECT_TRUE(IsErr(result));
  auto&& error = UnwrapErr(std::move(result));
  EXPECT_EQ(error.Code(), ToInt32(ErrorCode::kStreamFail));
  EXPECT_EQ(error.Message(), "Stream state is fail.");
}

TEST(StreamReader, Eof) {
  std::istringstream stream("");
  StreamReader reader(stream);
  auto&& result = reader.Read();
  EXPECT_TRUE(IsOk(result));
  auto&& item = Unwrap(std::move(result));
  EXPECT_TRUE(IsEof(item));
  auto&& eof = GetEof(item);
  EXPECT_EQ(eof.position, 0);
  EXPECT_EQ(reader.Position(), 0);
  EXPECT_EQ(stream.tellg(), -1);
}

TEST(StreamReader, Byte) {
  std::istringstream stream("a");
  StreamReader reader(stream);
  {
    auto&& result = reader.Read();
    EXPECT_TRUE(IsOk(result));
    auto&& item = Unwrap(std::move(result));
    EXPECT_TRUE(IsByte(item));
    auto&& byte = GetByte(item);
    EXPECT_EQ(byte.value, 'a');
    EXPECT_EQ(byte.position, 0);
    EXPECT_EQ(reader.Position(), 1);
    EXPECT_EQ(stream.tellg(), 1);
  }
  {
    auto&& result = reader.Read();
    EXPECT_TRUE(IsOk(result));
    auto&& item = Unwrap(std::move(result));
    EXPECT_TRUE(IsEof(item));
    auto&& eof = GetEof(item);
    EXPECT_EQ(eof.position, 1);
    EXPECT_EQ(reader.Position(), 1);
    EXPECT_EQ(stream.tellg(), -1);
  }
}

}  // namespace graphql_cpp
