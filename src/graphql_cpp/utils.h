#ifndef GRAPHQL_CPP_UTILS_H_
#define GRAPHQL_CPP_UTILS_H_

#include <functional>
#include <string_view>
#include <type_traits>

namespace graphql_cpp {

template <typename T>
concept Int32Convertible =
    std::is_convertible_v<T, std::int32_t> ||
    (std::is_enum_v<T> &&
     std::is_same_v<std::underlying_type_t<T>, std::int32_t>);

struct StringHash : public std::hash<std::string_view> {
  using is_transparent = void;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTILS_H_
