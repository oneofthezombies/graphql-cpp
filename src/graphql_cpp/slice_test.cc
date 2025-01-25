#include "graphql_cpp/slice.h"

#include <gtest/gtest.h>

namespace graphql_cpp {

TEST(SliceTest, NullData) { ASSERT_DEATH(Slice<int>(nullptr, 0), ""); }

TEST(SliceTest, Empty) {
  int data[0] = {};
  auto result = Slice<int>(data, 0);
  EXPECT_EQ(result.data(), data);
  EXPECT_TRUE(result.empty());
  EXPECT_EQ(result.size(), 0);
  ASSERT_DEATH(result[0], "");
}

TEST(SliceTest, Length1) {
  int data[1] = {1};
  auto result = Slice<int>(data, 1);
  EXPECT_EQ(result.data(), data);
  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], 1);
  ASSERT_DEATH(result[1], "");
}

TEST(SliceTest, Length2) {
  int data[2] = {1, 2};
  auto result = Slice<int>(data, 2);
  EXPECT_EQ(result.data(), data);
  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[1], 2);
  ASSERT_DEATH(result[2], "");
}

}  // namespace graphql_cpp
