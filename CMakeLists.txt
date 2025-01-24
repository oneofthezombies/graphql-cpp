cmake_minimum_required(VERSION 3.20.6)
project(graphqlcpp)

add_library(graphqlcpp src/language.cc)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(graphqlcpp_test src/language_test.cc)
target_link_libraries(graphqlcpp_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(graphqlcpp_test)
