#ifndef GRAPHQL_CPP_RESULT_H_
#define GRAPHQL_CPP_RESULT_H_

#include <cassert>
#include <variant>

namespace graphql_cpp {

template <typename T, typename E>
using Result = std::variant<T, E>;

template <typename T, typename E>
constexpr bool IsOk(const Result<T, E>& result) noexcept {
  return std::holds_alternative<T>(result);
}

template <typename T, typename E>
constexpr bool IsErr(const Result<T, E>& result) noexcept {
  return std::holds_alternative<E>(result);
}

template <typename T, typename E>
constexpr T& Unwrap(Result<T, E>& result) noexcept {
  assert(IsOk(result));
  return std::get<T>(result);
}

template <typename T, typename E>
constexpr const T& Unwrap(const Result<T, E>& result) noexcept {
  assert(IsOk(result));
  return std::get<T>(result);
}

template <typename T, typename E>
constexpr E& UnwrapErr(Result<T, E>& result) noexcept {
  assert(IsErr(result));
  return std::get<E>(result);
}

template <typename T, typename E>
constexpr const E& UnwrapErr(const Result<T, E>& result) noexcept {
  assert(IsErr(result));
  return std::get<E>(result);
}

template <typename T, typename E>
constexpr Result<T, E> Ok(T& value) noexcept {
  return Result<T, E>(value);
}

template <typename T, typename E>
constexpr Result<T, E> Ok(const T& value) noexcept {
  return Result<T, E>(value);
}

template <typename T, typename E>
constexpr Result<T, E> Ok(T&& value) noexcept {
  return Result<T, E>(std::move(value));
}

template <typename T, typename E>
constexpr Result<T, E> Ok(const T&& value) noexcept {
  return Result<T, E>(std::move(value));
}

template <typename T, typename E>
constexpr Result<T, E> Err(E& error) noexcept {
  return Result<T, E>(error);
}

template <typename T, typename E>
constexpr Result<T, E> Err(const E& error) noexcept {
  return Result<T, E>(error);
}

template <typename T, typename E>
constexpr Result<T, E> Err(E&& error) noexcept {
  return Result<T, E>(std::move(error));
}

template <typename T, typename E>
constexpr Result<T, E> Err(const E&& error) noexcept {
  return Result<T, E>(std::move(error));
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_RESULT_H_
