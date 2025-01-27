#ifndef GRAPHQL_CPP_RESULT_H_
#define GRAPHQL_CPP_RESULT_H_

#include <cassert>
#include <variant>

namespace graphql_cpp {

template <typename T, typename E>
using Result = std::variant<T, E>;

template <typename T, typename E>
[[nodiscard]] constexpr Result<T, E> Ok(const T& v) noexcept {
  return Result<T, E>{std::in_place_index<0>, v};
}

template <typename T, typename E>
[[nodiscard]] constexpr Result<T, E> Ok(T&& v) noexcept {
  return Result<T, E>{std::in_place_index<0>, std::move(v)};
}

template <typename T, typename E>
[[nodiscard]] constexpr Result<T, E> Err(const E& e) noexcept {
  return Result<T, E>{std::in_place_index<1>, e};
}

template <typename T, typename E>
[[nodiscard]] constexpr Result<T, E> Err(E&& e) noexcept {
  return Result<T, E>{std::in_place_index<1>, std::move(e)};
}

template <typename T, typename E>
[[nodiscard]] constexpr bool IsOk(const Result<T, E>& r) noexcept {
  return r.index() == 0;
}

template <typename T, typename E>
[[nodiscard]] constexpr bool IsErr(const Result<T, E>& r) noexcept {
  return r.index() == 1;
}

template <typename T, typename E>
[[nodiscard]] constexpr const T& Unwrap(const Result<T, E>& r) noexcept {
  assert(IsOk(r));
  return std::get<0>(r);
}

template <typename T, typename E>
[[nodiscard]] constexpr T&& Unwrap(Result<T, E>&& r) noexcept {
  assert(IsOk(r));
  return std::get<0>(std::move(r));
}

template <typename T, typename E>
[[nodiscard]] constexpr const E& UnwrapErr(const Result<T, E>& r) noexcept {
  assert(IsErr(r));
  return std::get<1>(r);
}

template <typename T, typename E>
[[nodiscard]] constexpr E&& UnwrapErr(Result<T, E>&& r) noexcept {
  assert(IsErr(r));
  return std::get<1>(std::move(r));
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr Result<std::invoke_result_t<F, T>, E> Map(
    const Result<T, E>& r, F op) noexcept {
  using T2 = std::invoke_result_t<F, T>;

  if (IsOk(r)) {
    return Result<T2, E>{std::in_place_index<0>, op(Unwrap(r))};
  } else {
    return Result<T2, E>{std::in_place_index<1>, UnwrapErr(r)};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr Result<std::invoke_result_t<F, T>, E> Map(
    Result<T, E>&& r, F op) noexcept {
  using T2 = std::invoke_result_t<F, T>;

  if (IsOk(r)) {
    return Result<T2, E>{std::in_place_index<0>, op(Unwrap(std::move(r)))};
  } else {
    return Result<T2, E>{std::in_place_index<1>, UnwrapErr(std::move(r))};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr Result<T, std::invoke_result_t<F, E>> MapErr(
    const Result<T, E>& r, F op) noexcept {
  using E2 = std::invoke_result_t<F, E>;

  if (IsErr(r)) {
    return Result<T, E2>{std::in_place_index<1>, op(std::move(UnwrapErr(r)))};
  } else {
    return Result<T, E2>{std::in_place_index<0>, Unwrap(r)};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr Result<T, std::invoke_result_t<F, E>> MapErr(
    Result<T, E>&& r, F op) noexcept {
  using E2 = std::invoke_result_t<F, E>;

  if (IsErr(r)) {
    return Result<T, E2>{std::in_place_index<1>, op(UnwrapErr(std::move(r)))};
  } else {
    return Result<T, E2>{std::in_place_index<0>, Unwrap(std::move(r))};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr std::invoke_result_t<F, T> AndThen(
    const Result<T, E>& r, F op) noexcept {
  using R = std::invoke_result_t<F, T>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(
      std::is_same_v<std::variant_alternative_t<1, R>, E>,
      "Second type of return type must be the same as the error type");

  if (IsOk(r)) {
    return op(Unwrap(r));
  } else {
    return R{std::in_place_index<1>, UnwrapErr(r)};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr std::invoke_result_t<F, T> AndThen(Result<T, E>&& r,
                                                           F op) noexcept {
  using R = std::invoke_result_t<F, T>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(
      std::is_same_v<std::variant_alternative_t<1, R>, E>,
      "Second type of return type must be the same as the error type");

  if (IsOk(r)) {
    return op(Unwrap(std::move(r)));
  } else {
    return R{std::in_place_index<1>, UnwrapErr(std::move(r))};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr std::invoke_result_t<F, E> OrElse(const Result<T, E>& r,
                                                          F op) noexcept {
  using R = std::invoke_result_t<F, E>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(std::is_same_v<std::variant_alternative_t<0, R>, T>,
                "First type of return type must be the same as the value type");

  if (IsErr(r)) {
    return op(UnwrapErr(r));
  } else {
    return R{std::in_place_index<0>, Unwrap(r)};
  }
}

template <typename T, typename E, typename F>
[[nodiscard]] constexpr std::invoke_result_t<F, E> OrElse(Result<T, E>&& r,
                                                          F op) noexcept {
  using R = std::invoke_result_t<F, E>;

  static_assert(std::variant_size_v<R> == 2,
                "Return type must be a Result (variant with two types)");
  static_assert(std::is_same_v<std::variant_alternative_t<0, R>, T>,
                "First type of return type must be the same as the value type");

  if (IsErr(r)) {
    return op(UnwrapErr(std::move(r)));
  } else {
    return R{std::in_place_index<0>, Unwrap(std::move(r))};
  }
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_RESULT_H_
