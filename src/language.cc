#include "language.h"

#include "utilities.h"

namespace graphql_cpp {

SourceText::SourceText(const Slice<std::uint8_t> bytes) noexcept
    : utf8_decoder_(bytes) {}

SourceText::NextSourceCharacter::Result
SourceText::NextSourceCharacter() noexcept {
  auto res = utf8_decoder_.NextCodePoint();
  return std::visit(
      [](auto&& res) -> NextSourceCharacter::Result {
        using T = std::decay_t<decltype(res)>;
        if constexpr (
            std::is_same_v<T, Utf8Decoder::NextCodePoint::Eof> ||
            std::is_same_v<T, Utf8Decoder::NextCodePoint::FirstByteError> ||
            std::is_same_v<T, Utf8Decoder::NextCodePoint::OutOfRangeError> ||
            std::is_same_v<T,
                           Utf8Decoder::NextCodePoint::ContinuationByteError>) {
          return res;
        } else if constexpr (std::is_same_v<
                                 T, Utf8Decoder::NextCodePoint::CodePoint>) {
          if (res.value == 0x0009 || res.value == 0x000a ||
              res.value == 0x000d ||
              (0x0020 <= res.value && res.value <= 0xffff)) {
            return NextSourceCharacter::SourceCharacter{.code_point =
                                                            res.value};
          } else {
            return NextSourceCharacter::NonSourceCharacter{.code_point =
                                                               res.value};
          }
        } else {
          static_assert(always_false<T>, "Unhandled case");
        }
      },
      res);
}

}  // namespace graphql_cpp
