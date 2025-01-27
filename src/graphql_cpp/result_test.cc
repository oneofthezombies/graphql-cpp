#include "graphql_cpp/result.h"

#include <gtest/gtest.h>

namespace graphql_cpp {

TEST(ResultIsOk, Reference) {
  auto result = Ok<int, std::string>(1);
  EXPECT_TRUE(IsOk(result));
  EXPECT_FALSE(IsErr(result));
}

TEST(ResultIsOk, Rvalue) {
  auto result = Ok<int, std::string>(1);
  EXPECT_TRUE(IsOk(std::move(result)));
  EXPECT_TRUE(IsOk(result));
  EXPECT_FALSE(IsErr(std::move(result)));
  EXPECT_FALSE(IsErr(result));
}

TEST(ResultIsOk, SameValueErrorType) {
  auto result = Ok<int, int>(1);
  EXPECT_TRUE(IsOk(result));
  EXPECT_FALSE(IsErr(result));
}

TEST(ResultIsErr, Reference) {
  auto result = Err<int, std::string>("error");
  EXPECT_TRUE(IsErr(result));
  EXPECT_FALSE(IsOk(result));
}

TEST(ResultIsErr, Rvalue) {
  auto result = Err<int, std::string>("error");
  EXPECT_TRUE(IsErr(std::move(result)));
  EXPECT_TRUE(IsErr(result));
  EXPECT_FALSE(IsOk(std::move(result)));
  EXPECT_FALSE(IsOk(result));
}

TEST(ResultIsErr, SameValueErrorType) {
  auto result = Err<int, int>(1);
  EXPECT_TRUE(IsErr(result));
  EXPECT_FALSE(IsOk(result));
}

TEST(ResultUnwrap, Reference) {
  auto result = Ok<int, std::string>(1);
  EXPECT_EQ(Unwrap(result), 1);
}

TEST(ResultUnwrap, Rvalue) {
  auto result = Ok<int, std::string>(1);
  EXPECT_EQ(Unwrap(std::move(result)), 1);
}

TEST(ResultUnwrap, UniquePtr) {
  auto result = Ok<std::unique_ptr<int>, std::string>(std::make_unique<int>(1));
  EXPECT_EQ(*Unwrap(result), 1);
  EXPECT_EQ(*Unwrap(std::move(result)), 1);
  EXPECT_EQ(*Unwrap(result), 1);
  EXPECT_EQ(*Unwrap(std::move(result)), 1);
}

TEST(ResultUnwrap, SameValueErrorType) {
  auto result = Ok<int, int>(1);
  EXPECT_EQ(Unwrap(result), 1);
}

TEST(ResultUnwrapErr, Reference) {
  auto result = Err<int, std::string>("error");
  EXPECT_EQ(UnwrapErr(result), "error");
}

TEST(ResultUnwrapErr, Rvalue) {
  auto result = Err<int, std::string>("error");
  EXPECT_EQ(UnwrapErr(std::move(result)), "error");
}

TEST(ResultUnwrapErr, UniquePtr) {
  auto result = Err<int, std::unique_ptr<std::string>>(
      std::make_unique<std::string>("error"));
  EXPECT_EQ(*UnwrapErr(result), "error");
  EXPECT_EQ(*UnwrapErr(std::move(result)), "error");
  EXPECT_EQ(*UnwrapErr(result), "error");
  EXPECT_EQ(*UnwrapErr(std::move(result)), "error");
}

TEST(ResultUnwrapErr, SameValueErrorType) {
  auto result = Err<int, int>(1);
  EXPECT_EQ(UnwrapErr(result), 1);
}

TEST(ResultMap, OkReference) {
  auto result = Ok<int, std::string>(1);
  auto result2 = Map(result, [](int x) { return x * 2; });
  EXPECT_EQ(Unwrap(result2), 2);
}

TEST(ResultMap, OkRvalue) {
  auto result = Ok<int, std::string>(1);
  auto result2 = Map(std::move(result), [](int x) { return x * 2; });
  EXPECT_EQ(Unwrap(result2), 2);
}

TEST(ResultMap, ErrReference) {
  auto result = Err<int, std::string>("error");
  auto result2 = Map(result, [](int x) { return x * 2; });
  EXPECT_EQ(UnwrapErr(result2), "error");
}

TEST(ResultMap, ErrRvalue) {
  auto result = Err<int, std::string>("error");
  auto result2 = Map(std::move(result), [](int x) { return x * 2; });
  EXPECT_EQ(UnwrapErr(result2), "error");
}

TEST(ResultMap, SameValueErrorType) {
  auto result = Ok<int, int>(1);
  auto result2 = Map(result, [](int x) { return x * 2; });
  EXPECT_EQ(Unwrap(result2), 2);
}

TEST(ResultMap, OtherValueType) {
  auto result = Ok<int, std::string>(1);
  auto result2 = Map(result, [](int x) { return std::to_string(x); });
  EXPECT_EQ(Unwrap(result2), "1");
}

TEST(ResultMapErr, OkReference) {
  auto result = Ok<int, std::string>(1);
  auto result2 = MapErr(result, [](std::string x) { return x + "error"; });
  EXPECT_EQ(Unwrap(result2), 1);
}

TEST(ResultMapErr, OkRvalue) {
  auto result = Ok<int, std::string>(1);
  auto result2 =
      MapErr(std::move(result), [](std::string x) { return x + "error"; });
  EXPECT_EQ(Unwrap(result2), 1);
}

TEST(ResultMapErr, ErrReference) {
  auto result = Err<int, std::string>("error");
  auto result2 = MapErr(result, [](std::string x) { return x + "error"; });
  EXPECT_EQ(UnwrapErr(result2), "errorerror");
}

TEST(ResultMapErr, ErrRvalue) {
  auto result = Err<int, std::string>("error");
  auto result2 =
      MapErr(std::move(result), [](std::string x) { return x + "error"; });
  EXPECT_EQ(UnwrapErr(result2), "errorerror");
}

TEST(ResultMapErr, SameValueErrorType) {
  auto result = Err<int, int>(1);
  auto result2 = MapErr(result, [](int x) { return x * 2; });
  EXPECT_EQ(UnwrapErr(result2), 2);
}

TEST(ResultMapErr, OtherErrorType) {
  auto result = Err<int, std::string>("1");
  auto result2 = MapErr(result, [](std::string x) { return std::stoi(x); });
  EXPECT_EQ(UnwrapErr(result2), 1);
}

TEST(ResultAndThen, OkReference) {
  auto result = Ok<int, std::string>(1);
  auto result2 =
      AndThen(result, [](int x) { return Ok<int, std::string>(x * 2); });
  EXPECT_EQ(Unwrap(result2), 2);
}

TEST(ResultAndThen, OkRvalue) {
  auto result = Ok<int, std::string>(1);
  auto result2 = AndThen(std::move(result),
                         [](int x) { return Ok<int, std::string>(x * 2); });
  EXPECT_EQ(Unwrap(result2), 2);
}

TEST(ResultAndThen, ErrReference) {
  auto result = Err<int, std::string>("error");
  auto result2 =
      AndThen(result, [](int x) { return Ok<int, std::string>(x * 2); });
  EXPECT_EQ(UnwrapErr(result2), "error");
}

TEST(ResultAndThen, ErrRvalue) {
  auto result = Err<int, std::string>("error");
  auto result2 = AndThen(std::move(result),
                         [](int x) { return Ok<int, std::string>(x * 2); });
  EXPECT_EQ(UnwrapErr(result2), "error");
}

TEST(ResultAndThen, SameValueErrorType) {
  auto result = Ok<int, int>(1);
  auto result2 = AndThen(result, [](int x) { return Ok<int, int>(x * 2); });
  EXPECT_EQ(Unwrap(result2), 2);
}

TEST(ResultAndThen, OtherValueType) {
  auto result = Ok<int, std::string>(1);
  auto result2 = AndThen(result, [](int x) {
    return Ok<std::string, std::string>(std::to_string(x));
  });
  EXPECT_EQ(Unwrap(result2), "1");
}

TEST(ResultOrElse, OkReference) {
  auto result = Ok<int, std::string>(1);
  auto result2 = OrElse(
      result, [](std::string x) { return Err<int, std::string>(x + "error"); });
  EXPECT_EQ(Unwrap(result2), 1);
}

TEST(ResultOrElse, OkRvalue) {
  auto result = Ok<int, std::string>(1);
  auto result2 = OrElse(std::move(result), [](std::string x) {
    return Err<int, std::string>(x + "error");
  });
  EXPECT_EQ(Unwrap(result2), 1);
}

TEST(ResultOrElse, ErrReference) {
  auto result = Err<int, std::string>("error");
  auto result2 = OrElse(
      result, [](std::string x) { return Err<int, std::string>(x + "error"); });
  EXPECT_EQ(UnwrapErr(result2), "errorerror");
}

TEST(ResultOrElse, ErrRvalue) {
  auto result = Err<int, std::string>("error");
  auto result2 = OrElse(std::move(result), [](std::string x) {
    return Err<int, std::string>(x + "error");
  });
  EXPECT_EQ(UnwrapErr(result2), "errorerror");
}

TEST(ResultOrElse, SameValueErrorType) {
  auto result = Err<int, int>(1);
  auto result2 = OrElse(result, [](int x) { return Err<int, int>(x * 2); });
  EXPECT_EQ(UnwrapErr(result2), 2);
}

TEST(ResultOrElse, OtherErrorType) {
  auto result = Err<int, std::string>("1");
  auto result2 =
      OrElse(result, [](std::string x) { return Err<int, int>(std::stoi(x)); });
  EXPECT_EQ(UnwrapErr(result2), 1);
}

}  // namespace graphql_cpp
