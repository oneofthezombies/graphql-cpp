#ifndef GRAPHQL_CPP_ANY_ERROR_H_
#define GRAPHQL_CPP_ANY_ERROR_H_

#include <memory>
#include <source_location>
#include <string>
#include <string_view>

namespace graphql_cpp {

struct AnyError {
  std::int32_t code{kDefaultCode};
  std::string message{kDefaultMessage};
  std::source_location location;
  std::unique_ptr<AnyError> cause;

  static constexpr std::int32_t kDefaultCode = 1;
  static constexpr std::string_view kDefaultMessage =
      "An unexpected error occurred";

  AnyError(
      std::int32_t code, std::unique_ptr<AnyError> cause = nullptr,
      std::source_location location = std::source_location::current()) noexcept;
  AnyError(
      std::string_view message, std::unique_ptr<AnyError> cause = nullptr,
      std::source_location location = std::source_location::current()) noexcept;
  AnyError(
      std::int32_t code, std::string_view message,
      std::unique_ptr<AnyError> cause = nullptr,
      std::source_location location = std::source_location::current()) noexcept;

  ~AnyError() noexcept = default;

  AnyError(const AnyError& other) = delete;
  AnyError& operator=(const AnyError& other) = delete;

  AnyError(AnyError&& other) noexcept = default;
  AnyError& operator=(AnyError&& other) noexcept = default;

  std::unique_ptr<AnyError> into_unique() noexcept;

  operator std::unique_ptr<AnyError>() noexcept;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_ANY_ERROR_H_
