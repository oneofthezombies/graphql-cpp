#ifndef GRAPHQL_CPP_UTILS_H_
#define GRAPHQL_CPP_UTILS_H_

#include <functional>
#include <string_view>
#include <type_traits>
#include <variant>

namespace graphql_cpp {

template <typename T>
concept Int32Convertible =
    std::is_convertible_v<T, std::int32_t> ||
    (std::is_enum_v<T> &&
     std::is_same_v<std::underlying_type_t<T>, std::int32_t>);

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

template <typename Variant, typename Visitor>
constexpr bool IsExhaustiveVisitor() {
  return ([]<std::size_t... I>(std::index_sequence<I...>, Visitor&& visitor) {
    return (
        std::is_invocable_v<Visitor, std::variant_alternative_t<I, Variant>> &&
        ...);
  })(std::make_index_sequence<std::variant_size_v<Variant>>{},
     std::forward<Visitor>(Visitor{}));
}

template <typename Variant, typename... Handlers>
auto MakeVisitor(Handlers&&... handlers) {
  auto visitor = Visitor{std::forward<Handlers>(handlers)...};
  static_assert(IsExhaustiveVisitor<Variant, decltype(visitor)>(),
                "Visitor does not handle all alternatives in the variant!");
  return visitor;
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTILS_H_
