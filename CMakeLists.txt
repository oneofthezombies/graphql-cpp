cmake_minimum_required(VERSION 3.20)
project(graphql_cpp)

add_library(graphql_cpp src/graphql_cpp/lexer.cc)
target_include_directories(graphql_cpp PUBLIC src)

if (BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_executable(
    graphql_cpp_test
    src/graphql_cpp/lexer_test.cc)
  target_link_libraries(graphql_cpp_test graphql_cpp GTest::gtest_main
                        GTest::gmock_main stdc++ m)

  include(GoogleTest)
  gtest_discover_tests(graphql_cpp_test)
endif()
