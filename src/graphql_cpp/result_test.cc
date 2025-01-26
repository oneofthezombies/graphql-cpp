#include "graphql_cpp/result.h"

#include <gtest/gtest.h>

#include "gtest/gtest.h"

namespace graphql_cpp {

TEST(result_is_ok, reference) {
  auto result = ok<int, std::string>(1);
  EXPECT_TRUE(is_ok(result));
  EXPECT_FALSE(is_err(result));
}

TEST(result_is_ok, rvalue) {
  auto result = ok<int, std::string>(1);
  EXPECT_TRUE(is_ok(std::move(result)));
  EXPECT_TRUE(is_ok(result));
  EXPECT_FALSE(is_err(std::move(result)));
  EXPECT_FALSE(is_err(result));
}

TEST(result_is_ok, same_value_error_type) {
  auto result = ok<int, int>(1);
  EXPECT_TRUE(is_ok(result));
  EXPECT_FALSE(is_err(result));
}

TEST(result_is_err, reference) {
  auto result = err<int, std::string>("error");
  EXPECT_TRUE(is_err(result));
  EXPECT_FALSE(is_ok(result));
}

TEST(result_is_err, rvalue) {
  auto result = err<int, std::string>("error");
  EXPECT_TRUE(is_err(std::move(result)));
  EXPECT_TRUE(is_err(result));
  EXPECT_FALSE(is_ok(std::move(result)));
  EXPECT_FALSE(is_ok(result));
}

TEST(result_is_err, same_value_error_type) {
  auto result = err<int, int>(1);
  EXPECT_TRUE(is_err(result));
  EXPECT_FALSE(is_ok(result));
}

TEST(result_unwrap, reference) {
  auto result = ok<int, std::string>(1);
  EXPECT_EQ(unwrap(result), 1);
}

TEST(result_unwrap, rvalue) {
  auto result = ok<int, std::string>(1);
  EXPECT_EQ(unwrap(std::move(result)), 1);
}

TEST(result_unwrap, unique_ptr) {
  auto result = ok<std::unique_ptr<int>, std::string>(std::make_unique<int>(1));
  EXPECT_EQ(*unwrap(result), 1);
  EXPECT_EQ(*unwrap(std::move(result)), 1);
  EXPECT_EQ(*unwrap(result), 1);
  EXPECT_EQ(*unwrap(std::move(result)), 1);
}

TEST(result_unwrap, same_value_error_type) {
  auto result = ok<int, int>(1);
  EXPECT_EQ(unwrap(result), 1);
}

TEST(result_unwrap_err, reference) {
  auto result = err<int, std::string>("error");
  EXPECT_EQ(unwrap_err(result), "error");
}

TEST(result_unwrap_err, rvalue) {
  auto result = err<int, std::string>("error");
  EXPECT_EQ(unwrap_err(std::move(result)), "error");
}

TEST(result_unwrap_err, unique_ptr) {
  auto result = err<int, std::unique_ptr<std::string>>(
      std::make_unique<std::string>("error"));
  EXPECT_EQ(*unwrap_err(result), "error");
  EXPECT_EQ(*unwrap_err(std::move(result)), "error");
  EXPECT_EQ(*unwrap_err(result), "error");
  EXPECT_EQ(*unwrap_err(std::move(result)), "error");
}

TEST(result_unwrap_err, same_value_error_type) {
  auto result = err<int, int>(1);
  EXPECT_EQ(unwrap_err(result), 1);
}

TEST(result_map, ok_reference) {
  auto result = ok<int, std::string>(1);
  auto result2 = map(result, [](int x) { return x * 2; });
  EXPECT_EQ(unwrap(result2), 2);
}

TEST(result_map, ok_rvalue) {
  auto result = ok<int, std::string>(1);
  auto result2 = map(std::move(result), [](int x) { return x * 2; });
  EXPECT_EQ(unwrap(result2), 2);
}

TEST(result_map, err_reference) {
  auto result = err<int, std::string>("error");
  auto result2 = map(result, [](int x) { return x * 2; });
  EXPECT_EQ(unwrap_err(result2), "error");
}

TEST(result_map, err_rvalue) {
  auto result = err<int, std::string>("error");
  auto result2 = map(std::move(result), [](int x) { return x * 2; });
  EXPECT_EQ(unwrap_err(result2), "error");
}

TEST(result_map, same_value_error_type) {
  auto result = ok<int, int>(1);
  auto result2 = map(result, [](int x) { return x * 2; });
  EXPECT_EQ(unwrap(result2), 2);
}

TEST(result_map, other_value_type) {
  auto result = ok<int, std::string>(1);
  auto result2 = map(result, [](int x) { return std::to_string(x); });
  EXPECT_EQ(unwrap(result2), "1");
}

TEST(result_map_err, ok_reference) {
  auto result = ok<int, std::string>(1);
  auto result2 = map_err(result, [](std::string x) { return x + "error"; });
  EXPECT_EQ(unwrap(result2), 1);
}

TEST(result_map_err, ok_rvalue) {
  auto result = ok<int, std::string>(1);
  auto result2 =
      map_err(std::move(result), [](std::string x) { return x + "error"; });
  EXPECT_EQ(unwrap(result2), 1);
}

TEST(result_map_err, err_reference) {
  auto result = err<int, std::string>("error");
  auto result2 = map_err(result, [](std::string x) { return x + "error"; });
  EXPECT_EQ(unwrap_err(result2), "errorerror");
}

TEST(result_map_err, err_rvalue) {
  auto result = err<int, std::string>("error");
  auto result2 =
      map_err(std::move(result), [](std::string x) { return x + "error"; });
  EXPECT_EQ(unwrap_err(result2), "errorerror");
}

TEST(result_map_err, same_value_error_type) {
  auto result = err<int, int>(1);
  auto result2 = map_err(result, [](int x) { return x * 2; });
  EXPECT_EQ(unwrap_err(result2), 2);
}

TEST(result_map_err, other_error_type) {
  auto result = err<int, std::string>("1");
  auto result2 = map_err(result, [](std::string x) { return std::stoi(x); });
  EXPECT_EQ(unwrap_err(result2), 1);
}

TEST(result_and_then, ok_reference) {
  auto result = ok<int, std::string>(1);
  auto result2 =
      and_then(result, [](int x) { return ok<int, std::string>(x * 2); });
  EXPECT_EQ(unwrap(result2), 2);
}

TEST(result_and_then, ok_rvalue) {
  auto result = ok<int, std::string>(1);
  auto result2 = and_then(std::move(result),
                          [](int x) { return ok<int, std::string>(x * 2); });
  EXPECT_EQ(unwrap(result2), 2);
}

TEST(result_and_then, err_reference) {
  auto result = err<int, std::string>("error");
  auto result2 =
      and_then(result, [](int x) { return ok<int, std::string>(x * 2); });
  EXPECT_EQ(unwrap_err(result2), "error");
}

TEST(result_and_then, err_rvalue) {
  auto result = err<int, std::string>("error");
  auto result2 = and_then(std::move(result),
                          [](int x) { return ok<int, std::string>(x * 2); });
  EXPECT_EQ(unwrap_err(result2), "error");
}

TEST(result_and_then, same_value_error_type) {
  auto result = ok<int, int>(1);
  auto result2 = and_then(result, [](int x) { return ok<int, int>(x * 2); });
  EXPECT_EQ(unwrap(result2), 2);
}

TEST(result_and_then, other_value_type) {
  auto result = ok<int, std::string>(1);
  auto result2 = and_then(result, [](int x) {
    return ok<std::string, std::string>(std::to_string(x));
  });
  EXPECT_EQ(unwrap(result2), "1");
}

TEST(result_or_else, ok_reference) {
  auto result = ok<int, std::string>(1);
  auto result2 = or_else(
      result, [](std::string x) { return err<int, std::string>(x + "error"); });
  EXPECT_EQ(unwrap(result2), 1);
}

TEST(result_or_else, ok_rvalue) {
  auto result = ok<int, std::string>(1);
  auto result2 = or_else(std::move(result), [](std::string x) {
    return err<int, std::string>(x + "error");
  });
  EXPECT_EQ(unwrap(result2), 1);
}

TEST(result_or_else, err_reference) {
  auto result = err<int, std::string>("error");
  auto result2 = or_else(
      result, [](std::string x) { return err<int, std::string>(x + "error"); });
  EXPECT_EQ(unwrap_err(result2), "errorerror");
}

TEST(result_or_else, err_rvalue) {
  auto result = err<int, std::string>("error");
  auto result2 = or_else(std::move(result), [](std::string x) {
    return err<int, std::string>(x + "error");
  });
  EXPECT_EQ(unwrap_err(result2), "errorerror");
}

TEST(result_or_else, same_value_error_type) {
  auto result = err<int, int>(1);
  auto result2 = or_else(result, [](int x) { return err<int, int>(x * 2); });
  EXPECT_EQ(unwrap_err(result2), 2);
}

TEST(result_or_else, other_error_type) {
  auto result = err<int, std::string>("1");
  auto result2 = or_else(
      result, [](std::string x) { return err<int, int>(std::stoi(x)); });
  EXPECT_EQ(unwrap_err(result2), 1);
}

}  // namespace graphql_cpp
