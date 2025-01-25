#ifndef GRAPHQL_CPP_UTILS_H_
#define GRAPHQL_CPP_UTILS_H_

#include <variant>

namespace graphql_cpp {

template <typename Value, typename Error>
using Result = std::variant<Value, Error>;

template <typename Value, typename Error>
bool IsOk(const Result<Value, Error>& result) noexcept {
  return std::holds_alternative<Value>(result);
}

template <typename Value, typename Error>
bool IsErr(const Result<Value, Error>& result) noexcept {
  return std::holds_alternative<Error>(result);
}

template <typename Value, typename Error>
Value Unwrap(const Result<Value, Error>& result) {
  return std::get<Value>(result);
}

template <typename Value, typename Error>
Error UnwrapErr(const Result<Value, Error>& result) {
  return std::get<Error>(result);
}

/**
 * Triggers a compile-time error for unhandled `std::variant` alternatives in
 * `std::visit`.
 */
template <typename T>
constexpr bool always_false = false;

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTILS_H_
