#include "graphql_cpp/lexer.h"

namespace graphql_cpp {

// Reference: https://spec.graphql.org/October2021/#sec-Language.Source-Text
bool IsSourceCharacter(std::uint32_t code_point) noexcept {
  return code_point == 0x0009 || code_point == 0x000a || code_point == 0x000d ||
         (0x0020 <= code_point && code_point <= 0xffff);
}

Lexer::Lexer(std::istream& stream) noexcept : reader_(stream) {}

Lexer::NextResult Lexer::next() noexcept { return {}; }

}  // namespace graphql_cpp
