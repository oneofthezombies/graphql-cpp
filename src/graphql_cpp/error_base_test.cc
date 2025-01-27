#include "graphql_cpp/error_base.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace graphql_cpp {

using ::testing::HasSubstr;

enum class ErrorCode {
  kError1 = 1,
  kError2 = 2,
  kError3 = 3,
  kError4 = 4,
};

using Error = ErrorBase<ErrorCode, ErrorCode::kError1>;

TEST(ErrorBaseLocation, CompareLocationWithLiterals) {
  auto error = Error::Builder().Code(ErrorCode::kError2).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Location().file_name(),
              HasSubstr("src/graphql_cpp/error_base_test.cc"));
  EXPECT_STREQ(error.Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorBaseLocation_"
               "CompareLocationWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Location().line(), 20);
  EXPECT_EQ(error.Location().column(), 16);
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorBaseLocation, CompareLocationOfCauseWithLiterals) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error =
      Error::Builder().Code(ErrorCode::kError3).Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Location().file_name(),
              HasSubstr("src/graphql_cpp/error_base_test.cc"));
  EXPECT_STREQ(error.Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorBaseLocation_"
               "CompareLocationOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Location().line(), 37);
  EXPECT_EQ(error.Location().column(), 7);
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Cause()->Location().file_name(),
              HasSubstr("src/graphql_cpp/error_base_test.cc"));
  EXPECT_STREQ(error.Cause()->Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorBaseLocation_"
               "CompareLocationOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Cause()->Location().line(), 35);
  EXPECT_EQ(error.Cause()->Location().column(), 16);
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseLocation, CompareLocationOfCauseOfCauseWithLiterals) {
  auto cause_of_cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto cause = Error::Builder()
                   .Code(ErrorCode::kError3)
                   .Cause(std::move(cause_of_cause))
                   .Build();
  auto error =
      Error::Builder().Code(ErrorCode::kError4).Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError4);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Location().file_name(),
              HasSubstr("src/graphql_cpp/error_base_test.cc"));
  EXPECT_STREQ(error.Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorBaseLocation_"
               "CompareLocationOfCauseOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Location().line(), 68);
  EXPECT_EQ(error.Location().column(), 7);
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Cause()->Location().file_name(),
              HasSubstr("src/graphql_cpp/error_base_test.cc"));
  EXPECT_STREQ(error.Cause()->Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorBaseLocation_"
               "CompareLocationOfCauseOfCauseWithLiterals_Test::TestBody()");
  EXPECT_EQ(error.Cause()->Location().line(), 63);
  EXPECT_EQ(error.Cause()->Location().column(), 16);
  EXPECT_EQ(error.Cause()->Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_THAT(error.Cause()->Cause()->Location().file_name(),
              HasSubstr("src/graphql_cpp/error_base_test.cc"));
  EXPECT_STREQ(error.Cause()->Cause()->Location().function_name(),
               "virtual void "
               "graphql_cpp::ErrorBaseLocation_"
               "CompareLocationOfCauseOfCauseWithLiterals_"
               "Test::TestBody()");
  EXPECT_EQ(error.Cause()->Cause()->Location().line(), 62);
  EXPECT_EQ(error.Cause()->Cause()->Location().column(), 25);
  EXPECT_EQ(error.Cause()->Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCode, WithoutCause) {
  auto error = Error::Builder().Code(ErrorCode::kError2).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCode, WithCauseWithCode) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error =
      Error::Builder().Code(ErrorCode::kError3).Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCode, WithCauseWithMessage) {
  auto cause = Error::Builder().Message("cause message").Build();
  auto error = Error::Builder()
                   .Code(ErrorCode::kError2)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCode, WithCauseWithCodeMessage) {
  auto cause = Error::Builder()
                   .Code(ErrorCode::kError2)
                   .Message("cause message")
                   .Build();
  auto error = Error::Builder()
                   .Code(ErrorCode::kError3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithMessage, WithoutCause) {
  auto error = Error::Builder().Message("test message").Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithMessage, WithCauseWithCode) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithMessage, WithCauseWithMessage) {
  auto cause = Error::Builder().Message("cause message").Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithMessage, WithCauseWithCodeMessage) {
  auto cause = Error::Builder()
                   .Code(ErrorCode::kError2)
                   .Message("cause message")
                   .Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCodeMessage, WithoutCause) {
  auto error =
      Error::Builder().Code(ErrorCode::kError2).Message("test message").Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCodeMessage, WithCauseWithCode) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error = Error::Builder()
                   .Code(ErrorCode::kError3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCodeMessage, WithCauseWithMessage) {
  auto cause = Error::Builder().Message("cause message").Build();
  auto error = Error::Builder()
                   .Code(ErrorCode::kError3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError1);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseConstructorWithCodeMessage, WithCauseWithCodeMessage) {
  auto cause = Error::Builder()
                   .Code(ErrorCode::kError2)
                   .Message("cause message")
                   .Build();
  auto error = Error::Builder()
                   .Code(ErrorCode::kError3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  EXPECT_EQ(error.Code(), ErrorCode::kError3);
  EXPECT_EQ(error.Message(), "test message");
  EXPECT_EQ(error.Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(error.Cause()->Message(), "cause message");
  EXPECT_EQ(error.Cause()->Cause(), nullptr);
}

TEST(ErrorBaseIntoUnique, ConstructorWithCodeWithoutCause) {
  auto error = Error::Builder().Code(ErrorCode::kError2).Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), ErrorCode::kError2);
  EXPECT_EQ(unique->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause(), nullptr);
}

TEST(ErrorBaseIntoUnique, ConstructorWithCodeWithCause) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error =
      Error::Builder().Code(ErrorCode::kError3).Cause(std::move(cause)).Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), ErrorCode::kError3);
  EXPECT_EQ(unique->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(unique->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Cause(), nullptr);
}

TEST(ErrorBaseIntoUnique, ConstructorWithMessageWithoutCause) {
  auto error = Error::Builder().Message("test message").Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), ErrorCode::kError1);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause(), nullptr);
}

TEST(ErrorBaseIntoUnique, ConstructorWithMessageWithCause) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error =
      Error::Builder().Message("test message").Cause(std::move(cause)).Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), ErrorCode::kError1);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(unique->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Cause(), nullptr);
}

TEST(ErrorBaseIntoUnique, ConstructorWithCodeMessageWithoutCause) {
  auto error =
      Error::Builder().Code(ErrorCode::kError2).Message("test message").Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), ErrorCode::kError2);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause(), nullptr);
}

TEST(ErrorBaseIntoUnique, ConstructorWithCodeMessageWithCause) {
  auto cause = Error::Builder().Code(ErrorCode::kError2).Build();
  auto error = Error::Builder()
                   .Code(ErrorCode::kError3)
                   .Message("test message")
                   .Cause(std::move(cause))
                   .Build();
  auto unique = error.ReleaseAsUnique();
  EXPECT_EQ(unique->Code(), ErrorCode::kError3);
  EXPECT_EQ(unique->Message(), "test message");
  EXPECT_EQ(unique->Cause()->Code(), ErrorCode::kError2);
  EXPECT_EQ(unique->Cause()->Message(), "An unexpected error occurred.");
  EXPECT_EQ(unique->Cause()->Cause(), nullptr);
}

}  // namespace graphql_cpp
