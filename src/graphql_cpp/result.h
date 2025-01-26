#ifndef GRAPHQL_CPP_RESULT_H_
#define GRAPHQL_CPP_RESULT_H_

#include <cassert>
#include <variant>

namespace graphql_cpp {

template <typename T, typename E>
using Result = std::variant<T, E>;

template <typename T, typename E>
constexpr Result<T, E> ok(const T& value) noexcept {
  return Result<T, E>{std::in_place_index<0>, value};
}

template <typename T, typename E>
constexpr Result<T, E> ok(T&& value) noexcept {
  return Result<T, E>{std::in_place_index<0>, std::move(value)};
}

template <typename T, typename E>
constexpr Result<T, E> err(const E& error) noexcept {
  return Result<T, E>{std::in_place_index<1>, error};
}

template <typename T, typename E>
constexpr Result<T, E> err(E&& error) noexcept {
  return Result<T, E>{std::in_place_index<1>, std::move(error)};
}

template <typename T, typename E>
constexpr bool is_ok(const Result<T, E>& result) noexcept {
  return result.index() == 0;
}

template <typename T, typename E>
constexpr bool is_err(const Result<T, E>& result) noexcept {
  return result.index() == 1;
}

template <typename T, typename E>
constexpr const T& unwrap(const Result<T, E>& result) noexcept {
  assert(is_ok(result));
  return std::get<0>(result);
}

template <typename T, typename E>
constexpr T&& unwrap(Result<T, E>&& result) noexcept {
  assert(is_ok(result));
  return std::get<0>(std::move(result));
}

template <typename T, typename E>
constexpr const E& unwrap_err(const Result<T, E>& result) noexcept {
  assert(is_err(result));
  return std::get<1>(result);
}

template <typename T, typename E>
constexpr E&& unwrap_err(Result<T, E>&& result) noexcept {
  assert(is_err(result));
  return std::get<1>(std::move(result));
}

template <typename T, typename E, typename F>
constexpr Result<std::invoke_result_t<F, T>, E> map(const Result<T, E>& result,
                                                    F op) noexcept {
  using T2 = std::invoke_result_t<F, T>;

  if (is_ok(result)) {
    return Result<T2, E>{std::in_place_index<0>, op(unwrap(result))};
  } else {
    return Result<T2, E>{std::in_place_index<1>, unwrap_err(result)};
  }
}

template <typename T, typename E, typename F>
constexpr Result<std::invoke_result_t<F, T>, E> map(Result<T, E>&& result,
                                                    F op) noexcept {
  using T2 = std::invoke_result_t<F, T>;

  if (is_ok(result)) {
    return Result<T2, E>{std::in_place_index<0>, op(unwrap(result))};
  } else {
    return Result<T2, E>{std::in_place_index<1>, unwrap_err(result)};
  }
}

template <typename T, typename E, typename F>
constexpr Result<T, std::invoke_result_t<F, E>> map_err(
    const Result<T, E>& result, F op) noexcept {
  using E2 = std::invoke_result_t<F, E>;

  if (is_err(result)) {
    return Result<T, E2>{std::in_place_index<1>, op(unwrap_err(result))};
  } else {
    return Result<T, E2>{std::in_place_index<0>, unwrap(result)};
  }
}

template <typename T, typename E, typename F>
constexpr Result<T, std::invoke_result_t<F, E>> map_err(Result<T, E>&& result,
                                                        F op) noexcept {
  using E2 = std::invoke_result_t<F, E>;

  if (is_err(result)) {
    return Result<T, E2>{std::in_place_index<1>, op(unwrap_err(result))};
  } else {
    return Result<T, E2>{std::in_place_index<0>, unwrap(result)};
  }
}

template <typename T, typename E, typename F>
constexpr std::invoke_result_t<F, T> and_then(const Result<T, E>& result,
                                              F op) noexcept {
  using R = std::invoke_result_t<F, T>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(
      std::is_same_v<std::variant_alternative_t<1, R>, E>,
      "Second type of return type must be the same as the error type");

  if (is_ok(result)) {
    return op(unwrap(result));
  } else {
    return R{std::in_place_index<1>, unwrap_err(result)};
  }
}

template <typename T, typename E, typename F>
constexpr std::invoke_result_t<F, T> and_then(Result<T, E>&& result,
                                              F op) noexcept {
  using R = std::invoke_result_t<F, T>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(
      std::is_same_v<std::variant_alternative_t<1, R>, E>,
      "Second type of return type must be the same as the error type");

  if (is_ok(result)) {
    return op(unwrap(result));
  } else {
    return R{std::in_place_index<1>, unwrap_err(result)};
  }
}

template <typename T, typename E, typename F>
constexpr std::invoke_result_t<F, E> or_else(const Result<T, E>& result,
                                             F op) noexcept {
  using R = std::invoke_result_t<F, E>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(std::is_same_v<std::variant_alternative_t<0, R>, T>,
                "First type of return type must be the same as the value type");

  if (is_err(result)) {
    return op(unwrap_err(result));
  } else {
    return R{std::in_place_index<0>, unwrap(result)};
  }
}

template <typename T, typename E, typename F>
constexpr std::invoke_result_t<F, E> or_else(Result<T, E>&& result,
                                             F op) noexcept {
  using R = std::invoke_result_t<F, E>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(std::is_same_v<std::variant_alternative_t<0, R>, T>,
                "First type of return type must be the same as the value type");

  if (is_err(result)) {
    return op(unwrap_err(result));
  } else {
    return R{std::in_place_index<0>, unwrap(result)};
  }
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_RESULT_H_
