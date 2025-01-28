#include "graphql_cpp/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace graphql_cpp {

using ::testing::HasSubstr;

TEST(ErrorLocation, CompareLocationWithLiterals) {
  auto error = Error::Builder().Code(2).Build();
  EXPECT_EQ(error.Code(), 2);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Location().file_name(),
              HasSubstr("src/graphql_cpp/error_test.cc"));
  EXPECT_STREQ(error.Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorLocation_"
               "CompareLocationWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Location().line(), 11);
  EXPECT_EQ(error.Location().column(), 16);
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorLocation, CompareLocationOfCauseWithLiterals) {
  auto cause = Error::Builder().Code(2).Build();
  auto error = Error::Builder().Code(3).Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), 3);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Location().file_name(),
              HasSubstr("src/graphql_cpp/error_test.cc"));
  EXPECT_STREQ(error.Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorLocation_"
               "CompareLocationOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Location().line(), 27);
  EXPECT_EQ(error.Location().column(), 16);
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Cause()->Location().file_name(),
              HasSubstr("src/graphql_cpp/error_test.cc"));
  EXPECT_STREQ(error.Cause()->Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorLocation_"
               "CompareLocationOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Cause()->Location().line(), 26);
  EXPECT_EQ(error.Cause()->Location().column(), 16);
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorLocation, CompareLocationOfCauseOfCauseWithLiterals) {
  auto cause_of_cause = Error::Builder().Code(2).Build();
  auto cause =
      Error::Builder().Code(3).Cause(std::move(cause_of_cause)).Build();
  auto error = Error::Builder().Code(4).Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), 4);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Location().file_name(),
              HasSubstr("src/graphql_cpp/error_test.cc"));
  EXPECT_STREQ(error.Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorLocation_"
               "CompareLocationOfCauseOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Location().line(), 55);
  EXPECT_EQ(error.Location().column(), 16);
  EXPECT_EQ(error.Cause()->Code(), 3);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Cause()->Location().file_name(),
              HasSubstr("src/graphql_cpp/error_test.cc"));
  EXPECT_STREQ(error.Cause()->Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorLocation_"
               "CompareLocationOfCauseOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Cause()->Location().line(), 54);
  EXPECT_EQ(error.Cause()->Location().column(), 7);
  EXPECT_EQ(error.Cause()->Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Cause()->Cause()->Location().file_name(),
              HasSubstr("src/graphql_cpp/error_test.cc"));
  EXPECT_STREQ(error.Cause()->Cause()->Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorLocation_"
               "CompareLocationOfCauseOfCauseWithLiterals_"
               "Test::TestBody()");
  EXPECT_EQ(error.Cause()->Cause()->Location().line(), 52);
  EXPECT_EQ(error.Cause()->Cause()->Location().column(), 25);
  EXPECT_EQ(error.Cause()->Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithCode, WithoutCause) {
  auto error = Error::Builder().Code(2).Build();
  EXPECT_EQ(error.Code(), 2);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorConstructorWithCode, WithCauseWithCode) {
  auto cause = Error::Builder().Code(2).Build();
  auto error = Error::Builder().Code(3).Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), 3);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithCode, WithCauseWithMessage) {
  auto cause = Error::Builder().Message("cause message").Build();
  auto error = Error::Builder()
                   .Code(2)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), 2);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 1);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithCode, WithCauseWithCodeMessage) {
  auto cause = Error::Builder().Code(2).Message("cause message").Build();
  auto error = Error::Builder()
                   .Code(3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), 3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithMessage, WithoutCause) {
  auto error = Error::Builder().Message("test message").Build();
  EXPECT_EQ(error.Code(), 1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorConstructorWithMessage, WithCauseWithCode) {
  auto cause = Error::Builder().Code(2).Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), 1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithMessage, WithCauseWithMessage) {
  auto cause = Error::Builder().Message("cause message").Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), 1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 1);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithMessage, WithCauseWithCodeMessage) {
  auto cause = Error::Builder().Code(2).Message("cause message").Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), 1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithCodeMessage, WithoutCause) {
  auto error = Error::Builder().Code(2).Message("test message").Build();
  EXPECT_EQ(error.Code(), 2);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorConstructorWithCodeMessage, WithCauseWithCode) {
  auto cause = Error::Builder().Code(2).Build();
  auto error = Error::Builder()
                   .Code(3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), 3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithCodeMessage, WithCauseWithMessage) {
  auto cause = Error::Builder().Message("cause message").Build();
  auto error = Error::Builder()
                   .Code(3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), 3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 1);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorConstructorWithCodeMessage, WithCauseWithCodeMessage) {
  auto cause = Error::Builder().Code(2).Message("cause message").Build();
  auto error = Error::Builder()
                   .Code(3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), 3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), 2);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorIntoUnique, ConstructorWithCodeWithoutCause) {
  auto error = Error::Builder().Code(2).Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), 2);
  EXPECT_EQ(unique->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause(), nullptr);
}

TEST(ErrorIntoUnique, ConstructorWithCodeWithCause) {
  auto cause = Error::Builder().Code(2).Build();
  auto error = Error::Builder().Code(3).Cause(std::move(cause)).Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), 3);
  EXPECT_EQ(unique->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Code(), 2);
  EXPECT_EQ(unique->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Cause(), nullptr);
}

TEST(ErrorIntoUnique, ConstructorWithMessageWithoutCause) {
  auto error = Error::Builder().Message("test message").Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), 1);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause(), nullptr);
}

TEST(ErrorIntoUnique, ConstructorWithMessageWithCause) {
  auto cause = Error::Builder().Code(2).Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), 1);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause()->Code(), 2);
  EXPECT_EQ(unique->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Cause(), nullptr);
}

TEST(ErrorIntoUnique, ConstructorWithCodeMessageWithoutCause) {
  auto error = Error::Builder().Code(2).Message("test message").Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), 2);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause(), nullptr);
}

TEST(ErrorIntoUnique, ConstructorWithCodeMessageWithCause) {
  auto cause = Error::Builder().Code(2).Build();
  auto error = Error::Builder()
                   .Code(3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), 3);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause()->Code(), 2);
  EXPECT_EQ(unique->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Cause(), nullptr);
}

}  // namespace graphql_cpp
