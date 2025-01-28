#ifndef GRAPHQL_CPP_RESULT_H_
#define GRAPHQL_CPP_RESULT_H_

#include <cassert>
#include <variant>

namespace graphql_cpp {

template <typename R>
concept ResultVariant = requires { typename std::variant_size<R>; } &&
                        (std::variant_size_v<R> == 2);

template <typename R, typename T>
concept ResultVariantWithValue = ResultVariant<R> && requires {
  requires std::is_same_v<std::variant_alternative_t<0, R>, T>;
};

template <typename R, typename E>
concept ResultVariantWithError = ResultVariant<R> && requires {
  requires std::is_same_v<std::variant_alternative_t<1, R>, E>;
};

template <typename T, typename E, typename F>
concept AndThenWithConstRef =
    std::invocable<F, const T&> &&
    ResultVariantWithError<std::invoke_result_t<F, const T&>, E>;

template <typename T, typename E, typename F>
concept AndThenWithMove =
    std::invocable<F, T&&> &&
    ResultVariantWithError<std::invoke_result_t<F, T&&>, E>;

template <typename T, typename E, typename F>
concept OrElseWithConstRef =
    std::invocable<F, const E&> &&
    ResultVariantWithValue<std::invoke_result_t<F, const E&>, T>;

template <typename T, typename E, typename F>
concept OrElseWithMove =
    std::invocable<F, E&&> &&
    ResultVariantWithValue<std::invoke_result_t<F, E&&>, T>;

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

namespace detail {

template <typename T2, typename E, typename R, typename F>
[[nodiscard]] constexpr auto Map(R&& r, F&& f) {
  if (IsOk(r)) {
    return Result<T2, E>{
        std::in_place_index<0>,
        std::invoke(std::forward<F>(f), Unwrap(std::forward<R>(r)))};
  } else {
    return Result<T2, E>{std::in_place_index<1>, UnwrapErr(std::forward<R>(r))};
  }
}

template <typename T, typename E2, typename R, typename F>
[[nodiscard]] constexpr auto MapErr(R&& r, F&& f) {
  if (IsErr(r)) {
    return Result<T, E2>{
        std::in_place_index<1>,
        std::invoke(std::forward<F>(f), UnwrapErr(std::forward<R>(r)))};
  } else {
    return Result<T, E2>{std::in_place_index<0>, Unwrap(std::forward<R>(r))};
  }
}

}  // namespace detail

template <typename T, typename E, typename F>
  requires std::invocable<F, const T&>
[[nodiscard]] constexpr auto Map(const Result<T, E>& r, F&& f) noexcept {
  using T2 = std::invoke_result_t<F, const T&>;
  return detail::Map<T2, E>(r, std::forward<F>(f));
}

template <typename T, typename E, typename F>
  requires std::invocable<F, T&&>
[[nodiscard]] constexpr auto Map(Result<T, E>&& r, F&& f) noexcept {
  using T2 = std::invoke_result_t<F, T&&>;
  return detail::Map<T2, E>(std::move(r), std::forward<F>(f));
}

template <typename T, typename E, typename F>
  requires std::invocable<F, const E&>
[[nodiscard]] constexpr auto MapErr(const Result<T, E>& r, F&& f) noexcept {
  using E2 = std::invoke_result_t<F, const E&>;
  return detail::MapErr<T, E2>(r, std::forward<F>(f));
}

template <typename T, typename E, typename F>
  requires std::invocable<F, E&&>
[[nodiscard]] constexpr auto MapErr(Result<T, E>&& r, F&& f) noexcept {
  using E2 = std::invoke_result_t<F, E&&>;
  return detail::MapErr<T, E2>(std::move(r), std::forward<F>(f));
}

namespace detail {

template <typename T, typename E, typename R2, typename R, typename F>
[[nodiscard]] constexpr auto AndThen(R&& r, F&& f) {
  if (IsOk(r)) {
    return f(Unwrap(std::forward<R>(r)));
  } else {
    return R2{std::in_place_index<1>, UnwrapErr(std::forward<R>(r))};
  }
}

template <typename T, typename E, typename R2, typename R, typename F>
[[nodiscard]] constexpr auto OrElse(R&& r, F&& f) {
  if (IsErr(r)) {
    return f(UnwrapErr(std::forward<R>(r)));
  } else {
    return R2{std::in_place_index<0>, Unwrap(std::forward<R>(r))};
  }
}

}  // namespace detail

template <typename T, typename E, typename F>
  requires AndThenWithConstRef<T, E, F>
[[nodiscard]] constexpr auto AndThen(const Result<T, E>& r, F&& f) noexcept {
  using R2 = std::invoke_result_t<F, const T&>;
  return detail::AndThen<T, E, R2>(r, std::forward<F>(f));
}

template <typename T, typename E, typename F>
  requires AndThenWithMove<T, E, F>
[[nodiscard]] constexpr auto AndThen(Result<T, E>&& r, F&& f) noexcept {
  using R2 = std::invoke_result_t<F, T&&>;
  return detail::AndThen<T, E, R2>(std::move(r), std::forward<F>(f));
}

template <typename T, typename E, typename F>
  requires OrElseWithConstRef<T, E, F>
[[nodiscard]] constexpr auto OrElse(const Result<T, E>& r, F&& f) noexcept {
  using R2 = std::invoke_result_t<F, const E&>;
  return detail::OrElse<T, E, R2>(r, std::forward<F>(f));
}

template <typename T, typename E, typename F>
  requires OrElseWithMove<T, E, F>
[[nodiscard]] constexpr auto OrElse(Result<T, E>&& r, F&& f) noexcept {
  using R2 = std::invoke_result_t<F, E&&>;
  return detail::OrElse<T, E, R2>(std::move(r), std::forward<F>(f));
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_RESULT_H_
