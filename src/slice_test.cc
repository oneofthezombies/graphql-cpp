#include "slice.h"

#include <gtest/gtest.h>

namespace graphql_cpp {

namespace slice {

TEST(SliceTest, NullData) {
  auto result = Slice<int>::Create(nullptr, 0);
  EXPECT_TRUE(std::holds_alternative<NullDataError>(result));
}

TEST(SliceTest, Valid) {
  int data[1] = {1};
  auto result = Slice<int>::Create(data, 1);
  EXPECT_TRUE(std::holds_alternative<Slice<int>>(result));
}

}  // namespace slice

}  // namespace graphql_cpp
