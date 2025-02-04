#include "graphql_cpp/source_text.h"

#include <gtest/gtest.h>

#include <sstream>

namespace graphql_cpp {

TEST(SourceTextIterator, StdString) {
  std::string s{"a"};
  auto begin = s.begin();
  SourceText<decltype(begin)> source(begin, s.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceTextIterator, StdStringView) {
  std::string_view s{"a"};
  auto begin = s.begin();
  SourceText<decltype(begin)> source(begin, s.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceTextIterator, StdVectorChar) {
  std::vector<char> src{'a'};
  auto begin = src.begin();
  SourceText<decltype(begin)> source(begin, src.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceTextIterator, StdArrayChar) {
  std::array<char, 1> src{{'a'}};
  auto begin = src.begin();
  SourceText<decltype(begin)> source(begin, src.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceTextIterator, StdSpanChar) {
  std::array<char, 1> src{{'a'}};
  std::span<char> s{src};
  auto begin = s.begin();
  SourceText<decltype(begin)> source(begin, s.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceTextIterator, StdIstringstreamIstreamIterator) {
  std::istringstream s{"a"};
  using CharType = decltype(s)::char_type;
  using Iterator = std::istream_iterator<CharType>;
  Iterator begin(s);
  Iterator end;
  SourceText<decltype(begin)> source(begin, end);
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceTextIterator, StdIstringstreamIstreambufIterator) {
  std::istringstream s{"a"};
  using CharType = decltype(s)::char_type;
  using Iterator = std::istreambuf_iterator<CharType>;
  Iterator begin(s.rdbuf());
  Iterator end;
  SourceText<decltype(begin)> source(begin, end);
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

}  // namespace graphql_cpp
