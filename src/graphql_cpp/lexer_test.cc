#include "graphql_cpp/lexer.h"

#include <gtest/gtest.h>

#include <sstream>

namespace graphql_cpp {

TEST(LexerSourceIterator, StdString) {
  std::string s{"a"};
  auto begin = s.begin();
  Lexer<decltype(begin)> lexer(begin, s.end());
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

TEST(LexerSourceIterator, StdStringView) {
  std::string_view s{"a"};
  auto begin = s.begin();
  Lexer<decltype(begin)> lexer(begin, s.end());
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

TEST(LexerSourceIterator, StdVectorChar) {
  std::vector<char> src{'a'};
  auto begin = src.begin();
  Lexer<decltype(begin)> lexer(begin, src.end());
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

TEST(LexerSourceIterator, StdArrayChar) {
  std::array<char, 1> src{{'a'}};
  auto begin = src.begin();
  Lexer<decltype(begin)> lexer(begin, src.end());
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

TEST(LexerSourceIterator, StdSpanChar) {
  std::array<char, 1> src{{'a'}};
  std::span<char> s{src};
  auto begin = s.begin();
  Lexer<decltype(begin)> lexer(begin, s.end());
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

TEST(LexerSourceIterator, StdIstringstreamIstreamIterator) {
  std::istringstream s{"a"};
  using CharType = decltype(s)::char_type;
  using Iterator = std::istream_iterator<CharType>;
  Iterator begin(s);
  Iterator end;
  Lexer<decltype(begin)> lexer(begin, end);
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

TEST(LexerSourceIterator, StdIstringstreamIstreambufIterator) {
  std::istringstream s{"a"};
  using CharType = decltype(s)::char_type;
  using Iterator = std::istreambuf_iterator<CharType>;
  Iterator begin(s.rdbuf());
  Iterator end;
  Lexer<decltype(begin)> lexer(begin, end);
  EXPECT_EQ(lexer.NextChar(), 'a');
  EXPECT_EQ(lexer.NextChar(), std::nullopt);
}

}  // namespace graphql_cpp
