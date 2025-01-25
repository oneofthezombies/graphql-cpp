#include "graphql_cpp/lexer.h"

#include <gtest/gtest.h>

namespace graphql_cpp {

TEST(LexerTest, AlwaysPasses) {
  Lexer lexer(std::make_unique<std::stringstream>(""));
  EXPECT_TRUE(true);
}

}  // namespace graphql_cpp
