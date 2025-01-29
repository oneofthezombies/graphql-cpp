cmake_minimum_required(VERSION 3.20.6)
project(graphqlcpp)

include(FetchContent)
FetchContent_Declare(
  kero_variant_utils
  GIT_REPOSITORY https://github.com/oneofthezombies/kero-variant-utils.git
  GIT_TAG v0.4.1)
FetchContent_MakeAvailable(kero_variant_utils)

add_library(graphqlcpp src/graphql_cpp/error.cc
                       src/graphql_cpp/stream_reader.cc src/graphql_cpp/utf8.cc)
target_include_directories(graphqlcpp PUBLIC src)
target_include_directories(graphqlcpp PRIVATE ${kero_variant_utils_SOURCE_DIR})

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  graphqlcpp_test
  src/graphql_cpp/slice_test.cc src/graphql_cpp/error_test.cc
  src/graphql_cpp/result_test.cc src/graphql_cpp/stream_reader_test.cc
  src/graphql_cpp/utf8_test.cc)
target_link_libraries(graphqlcpp_test graphqlcpp GTest::gtest_main
                      GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(graphqlcpp_test)
