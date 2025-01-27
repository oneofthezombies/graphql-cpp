#ifndef GRAPHQL_CPP_UTILS_H_
#define GRAPHQL_CPP_UTILS_H_

#include <string_view>
#include <type_traits>

namespace graphql_cpp {

/**
 * Triggers a compile-time error for unhandled `std::variant` alternatives in
 * `std::visit`.
 */
template <typename T>
constexpr bool always_false = false;

template <typename T>
struct is_enum_class {
  static constexpr bool value =
      std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;
};

template <typename T>
inline constexpr bool is_enum_class_v = is_enum_class<T>::value;

template <typename T>
concept EnumClass = is_enum_class_v<T>;

struct StringHash : public std::hash<std::string_view> {
  using is_transparent = void;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTILS_H_
