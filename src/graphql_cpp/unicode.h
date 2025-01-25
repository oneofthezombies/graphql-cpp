#ifndef GRAPHQL_CPP_UNICODE_H_
#define GRAPHQL_CPP_UNICODE_H_

#include <optional>

#include "graphql_cpp/slice.h"

namespace graphql_cpp {

[[nodiscard]] std::optional<std::size_t> Utf8BytesLength(
    std::uint8_t first_byte) noexcept;

[[nodiscard]] std::optional<std::uint32_t> Utf8CodePoint(
    Slice<std::uint8_t> bytes) noexcept;

[[nodiscard]] bool IsUtf8ContinuationByte(std::uint8_t byte) noexcept;

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UNICODE_H_
