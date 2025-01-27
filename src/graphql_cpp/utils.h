#ifndef GRAPHQL_CPP_UTILS_H_
#define GRAPHQL_CPP_UTILS_H_

#include <functional>
#include <string_view>
#include <type_traits>
#include <variant>

namespace graphql_cpp {

template <typename T>
constexpr bool always_false = false;

template <typename T>
struct IsEnumClass {
  static constexpr bool value =
      std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;
};

template <typename T>
inline constexpr bool is_enum_class_v = IsEnumClass<T>::value;

template <typename T>
concept EnumClass = is_enum_class_v<T>;

struct StringHash : public std::hash<std::string_view> {
  using is_transparent = void;
};

template <typename Variant, typename Visitor>
constexpr decltype(auto) Visit(Variant&& variant, Visitor&& visitor) {
  return std::visit(std::forward<Visitor>(visitor),
                    std::forward<Variant>(variant));
}

template <typename... Handlers>
struct Visitor : Handlers... {
  using Handlers::operator()...;
};

template <typename... Handlers>
Visitor(Handlers...) -> Visitor<Handlers...>;

template <typename Variant, typename... Handlers>
constexpr decltype(auto) Visit(Variant&& variant, Handlers&&... handlers) {
  return std::visit(Visitor<Handlers...>{std::forward<Handlers>(handlers)...},
                    std::forward<Variant>(variant));
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTILS_H_
