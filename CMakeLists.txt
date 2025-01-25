cmake_minimum_required(VERSION 3.20.6)
project(graphqlcpp)

add_library(src/graphql_cpp/utf8_stream.cc src/graphql_cpp/lexer.cc)
target_include_directories(graphqlcpp PUBLIC src)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  graphqlcpp_test
  src/graphql_cpp/slice_test.cc src/graphql_cpp/utf8_stream_test.cc
  src/graphql_cpp/lexer_test.cc)
target_link_libraries(graphqlcpp_test graphqlcpp GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(graphqlcpp_test)
