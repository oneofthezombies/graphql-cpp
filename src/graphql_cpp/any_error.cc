#include "graphql_cpp/any_error.h"

namespace graphql_cpp {

AnyError::AnyError(std::int32_t code, std::unique_ptr<AnyError> cause,
                   std::source_location location) noexcept
    : code{code}, cause{std::move(cause)}, location{location} {}

AnyError::AnyError(std::string message, std::unique_ptr<AnyError> cause,
                   std::source_location location) noexcept
    : message{std::move(message)},
      cause{std::move(cause)},
      location{location} {}

AnyError::AnyError(std::int32_t code, std::string message,
                   std::unique_ptr<AnyError> cause,
                   std::source_location location) noexcept
    : code{code},
      message{std::move(message)},
      cause{std::move(cause)},
      location{location} {}

std::unique_ptr<AnyError> AnyError::into_unique() noexcept {
  return std::make_unique<AnyError>(std::move(*this));
}

AnyError::operator std::unique_ptr<AnyError>() noexcept {
  return into_unique();
}

}  // namespace graphql_cpp
