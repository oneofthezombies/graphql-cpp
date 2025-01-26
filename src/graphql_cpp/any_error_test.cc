#include "graphql_cpp/any_error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace graphql_cpp {

using ::testing::HasSubstr;

TEST(any_error_location, compare_location_with_literals) {
  AnyError error{2};
  EXPECT_EQ(error.code, 2);
  EXPECT_EQ(error.message, "An unexpected error occurred");
  EXPECT_THAT(error.location.file_name(),
              HasSubstr("src/graphql_cpp/any_error_test.cc"));
  EXPECT_STREQ(error.location.function_name(),
               "virtual void "
               "graphql_cpp::any_error_location_compare_location_with_literals_"
               "Test::TestBody()");
  EXPECT_EQ(error.location.line(), 11);
  EXPECT_EQ(error.location.column(), 12);
  EXPECT_EQ(error.cause, nullptr);
}

TEST(any_error_location, compare_location_of_cause_with_literals) {
  AnyError cause{2};
  AnyError error{3, std::move(cause)};
  EXPECT_EQ(error.code, 3);
  EXPECT_EQ(error.message, "An unexpected error occurred");
  EXPECT_THAT(error.location.file_name(),
              HasSubstr("src/graphql_cpp/any_error_test.cc"));
  EXPECT_STREQ(error.location.function_name(),
               "virtual void "
               "graphql_cpp::any_error_location_compare_location_of_cause_with_"
               "literals_Test::TestBody()");
  EXPECT_EQ(error.location.line(), 27);
  EXPECT_EQ(error.location.column(), 12);
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "An unexpected error occurred");
  EXPECT_THAT(error.cause->location.file_name(),
              HasSubstr("src/graphql_cpp/any_error_test.cc"));
  EXPECT_STREQ(error.cause->location.function_name(),
               "virtual void "
               "graphql_cpp::any_error_location_compare_location_of_cause_with_"
               "literals_Test::TestBody()");
  EXPECT_EQ(error.cause->location.line(), 26);
  EXPECT_EQ(error.cause->location.column(), 12);
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_location, compare_location_of_cause_of_cause_with_literals) {
  AnyError cause_of_cause{2};
  AnyError cause{3, std::move(cause_of_cause)};
  AnyError error{4, std::move(cause)};
  EXPECT_EQ(error.code, 4);
  EXPECT_EQ(error.message, "An unexpected error occurred");
  EXPECT_THAT(error.location.file_name(),
              HasSubstr("src/graphql_cpp/any_error_test.cc"));
  EXPECT_STREQ(
      error.location.function_name(),
      "virtual void "
      "graphql_cpp::any_error_location_compare_location_of_cause_of_cause_"
      "with_literals_Test::TestBody()");
  EXPECT_EQ(error.location.line(), 54);
  EXPECT_EQ(error.location.column(), 12);
  EXPECT_EQ(error.cause->code, 3);
  EXPECT_EQ(error.cause->message, "An unexpected error occurred");
  EXPECT_THAT(error.cause->location.file_name(),
              HasSubstr("src/graphql_cpp/any_error_test.cc"));
  EXPECT_STREQ(
      error.cause->location.function_name(),
      "virtual void "
      "graphql_cpp::any_error_location_compare_location_of_cause_of_cause_"
      "with_literals_Test::TestBody()");
  EXPECT_EQ(error.cause->location.line(), 53);
  EXPECT_EQ(error.cause->location.column(), 12);
  EXPECT_EQ(error.cause->cause->code, 2);
  EXPECT_EQ(error.cause->cause->message, "An unexpected error occurred");
  EXPECT_THAT(error.cause->cause->location.file_name(),
              HasSubstr("src/graphql_cpp/any_error_test.cc"));
  EXPECT_STREQ(
      error.cause->cause->location.function_name(),
      "virtual void "
      "graphql_cpp::any_error_location_compare_location_of_cause_of_cause_"
      "with_literals_Test::TestBody()");
  EXPECT_EQ(error.cause->cause->location.line(), 52);
  EXPECT_EQ(error.cause->cause->location.column(), 12);
  EXPECT_EQ(error.cause->cause->cause, nullptr);
}

TEST(any_error_constructor_with_code, without_cause) {
  AnyError error{2};
  EXPECT_EQ(error.code, 2);
  EXPECT_EQ(error.message, "An unexpected error occurred");
  EXPECT_EQ(error.cause, nullptr);
}

TEST(any_error_constructor_with_code, with_cause_with_code) {
  AnyError cause{2};
  AnyError error{3, std::move(cause)};
  EXPECT_EQ(error.code, 3);
  EXPECT_EQ(error.message, "An unexpected error occurred");
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "An unexpected error occurred");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_code, with_cause_with_message) {
  AnyError cause{"cause message"};
  AnyError error{2, "test message", std::move(cause)};
  EXPECT_EQ(error.code, 2);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 1);
  EXPECT_EQ(error.cause->message, "cause message");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_code, with_cause_with_code_message) {
  AnyError cause{2, "cause message"};
  AnyError error{3, "test message", std::move(cause)};
  EXPECT_EQ(error.code, 3);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "cause message");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_message, without_cause) {
  AnyError error{"test message"};
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause, nullptr);
}

TEST(any_error_constructor_with_message, with_cause_with_code) {
  AnyError cause{2};
  AnyError error{"test message", std::move(cause)};
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "An unexpected error occurred");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_message, with_cause_with_message) {
  AnyError cause{"cause message"};
  AnyError error{"test message", std::move(cause)};
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 1);
  EXPECT_EQ(error.cause->message, "cause message");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_message, with_cause_with_code_message) {
  AnyError cause{2, "cause message"};
  AnyError error{"test message", std::move(cause)};
  EXPECT_EQ(error.code, 1);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "cause message");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_code_message, without_cause) {
  AnyError error{2, "test message"};
  EXPECT_EQ(error.code, 2);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause, nullptr);
}

TEST(any_error_constructor_with_code_message, with_cause_with_code) {
  AnyError cause{2};
  AnyError error{3, "test message", std::move(cause)};
  EXPECT_EQ(error.code, 3);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "An unexpected error occurred");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_code_message, with_cause_with_message) {
  AnyError cause{"cause message"};
  AnyError error{3, "test message", std::move(cause)};
  EXPECT_EQ(error.code, 3);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 1);
  EXPECT_EQ(error.cause->message, "cause message");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_constructor_with_code_message, with_cause_with_code_message) {
  AnyError cause{2, "cause message"};
  AnyError error{3, "test message", std::move(cause)};
  EXPECT_EQ(error.code, 3);
  EXPECT_EQ(error.message, "test message");
  EXPECT_EQ(error.cause->code, 2);
  EXPECT_EQ(error.cause->message, "cause message");
  EXPECT_EQ(error.cause->cause, nullptr);
}

TEST(any_error_into_unique, constructor_with_code_without_cause) {
  AnyError error{2};
  auto unique = error.into_unique();
  EXPECT_EQ(unique->code, 2);
  EXPECT_EQ(unique->message, "An unexpected error occurred");
  EXPECT_EQ(unique->cause, nullptr);
}

TEST(any_error_into_unique, constructor_with_code_with_cause) {
  AnyError cause{2};
  AnyError error{3, std::move(cause)};
  auto unique = error.into_unique();
  EXPECT_EQ(unique->code, 3);
  EXPECT_EQ(unique->message, "An unexpected error occurred");
  EXPECT_EQ(unique->cause->code, 2);
  EXPECT_EQ(unique->cause->message, "An unexpected error occurred");
  EXPECT_EQ(unique->cause->cause, nullptr);
}

TEST(any_error_into_unique, constructor_with_message_without_cause) {
  AnyError error{"test message"};
  auto unique = error.into_unique();
  EXPECT_EQ(unique->code, 1);
  EXPECT_EQ(unique->message, "test message");
  EXPECT_EQ(unique->cause, nullptr);
}

TEST(any_error_into_unique, constructor_with_message_with_cause) {
  AnyError cause{2};
  AnyError error{"test message", std::move(cause)};
  auto unique = error.into_unique();
  EXPECT_EQ(unique->code, 1);
  EXPECT_EQ(unique->message, "test message");
  EXPECT_EQ(unique->cause->code, 2);
  EXPECT_EQ(unique->cause->message, "An unexpected error occurred");
  EXPECT_EQ(unique->cause->cause, nullptr);
}

TEST(any_error_into_unique, constructor_with_code_message_without_cause) {
  AnyError error{2, "test message"};
  auto unique = error.into_unique();
  EXPECT_EQ(unique->code, 2);
  EXPECT_EQ(unique->message, "test message");
  EXPECT_EQ(unique->cause, nullptr);
}

TEST(any_error_into_unique, constructor_with_code_message_with_cause) {
  AnyError cause{2};
  AnyError error{3, "test message", std::move(cause)};
  auto unique = error.into_unique();
  EXPECT_EQ(unique->code, 3);
  EXPECT_EQ(unique->message, "test message");
  EXPECT_EQ(unique->cause->code, 2);
  EXPECT_EQ(unique->cause->message, "An unexpected error occurred");
  EXPECT_EQ(unique->cause->cause, nullptr);
}

}  // namespace graphql_cpp
