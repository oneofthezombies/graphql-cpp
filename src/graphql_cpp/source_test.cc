#include "graphql_cpp/source.h"

#include <gtest/gtest.h>

#include <sstream>

namespace graphql_cpp {

TEST(SourceIterator, StdString) {
  std::string s{"a"};
  auto begin = s.begin();
  Source<decltype(begin)> source(begin, s.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceIterator, StdStringView) {
  std::string_view s{"a"};
  auto begin = s.begin();
  Source<decltype(begin)> source(begin, s.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceIterator, StdVectorChar) {
  std::vector<char> src{'a'};
  auto begin = src.begin();
  Source<decltype(begin)> source(begin, src.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceIterator, StdArrayChar) {
  std::array<char, 1> src{{'a'}};
  auto begin = src.begin();
  Source<decltype(begin)> source(begin, src.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceIterator, StdSpanChar) {
  std::array<char, 1> src{{'a'}};
  std::span<char> s{src};
  auto begin = s.begin();
  Source<decltype(begin)> source(begin, s.end());
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceIterator, StdIstringstreamIstreamIterator) {
  std::istringstream s{"a"};
  using CharType = decltype(s)::char_type;
  using Iterator = std::istream_iterator<CharType>;
  Iterator begin(s);
  Iterator end;
  Source<decltype(begin)> source(begin, end);
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

TEST(SourceIterator, StdIstringstreamIstreambufIterator) {
  std::istringstream s{"a"};
  using CharType = decltype(s)::char_type;
  using Iterator = std::istreambuf_iterator<CharType>;
  Iterator begin(s.rdbuf());
  Iterator end;
  Source<decltype(begin)> source(begin, end);
  EXPECT_EQ(source.NextSourceChar(), 'a');
  EXPECT_EQ(source.NextSourceChar(), std::nullopt);
}

}  // namespace graphql_cpp
