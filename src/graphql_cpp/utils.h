#ifndef GRAPHQL_CPP_UTILS_H_
#define GRAPHQL_CPP_UTILS_H_

namespace graphql_cpp {

/**
 * Triggers a compile-time error for unhandled `std::variant` alternatives in
 * `std::visit`.
 */
template <typename T>
constexpr bool always_false = false;

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTILS_H_
