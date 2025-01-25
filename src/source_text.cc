#include "source_text.h"

#include "utilities.h"

namespace graphql_cpp {

namespace source_text {

SourceText::SourceText(const slice::Slice<std::uint8_t> text) noexcept
    : utf8_decoder_(text) {}

NextSourceCharacterResult SourceText::NextSourceCharacter() noexcept {
  auto res = utf8_decoder_.NextCodePoint();
  return std::visit(
      [](auto&& res) -> NextSourceCharacterResult {
        using T = std::decay_t<decltype(res)>;
        if constexpr (std::is_same_v<T, utf8_decoder::Eof> ||
                      std::is_same_v<T, utf8_decoder::FirstByteError> ||
                      std::is_same_v<T, utf8_decoder::CodePointLengthError> ||
                      std::is_same_v<T, utf8_decoder::ContinuationByteError>) {
          return res;
        } else if constexpr (std::is_same_v<T, utf8_decoder::CodePoint>) {
          if (res.value == 0x0009 || res.value == 0x000a ||
              res.value == 0x000d ||
              (0x0020 <= res.value && res.value <= 0xffff)) {
            return SourceCharacter{.code_point = res.value};
          } else {
            return NonSourceCharacter{.code_point = res.value};
          }
        } else {
          static_assert(always_false<T>, "Unhandled case");
        }
      },
      res);
}

}  // namespace source_text

}  // namespace graphql_cpp
