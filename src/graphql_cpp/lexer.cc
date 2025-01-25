#include "graphql_cpp/lexer.h"

namespace graphql_cpp {

bool IsSourceCharacter(std::uint32_t code_point) noexcept {
  return code_point == 0x0009 || code_point == 0x000a || code_point == 0x000d ||
         (0x0020 <= code_point && code_point <= 0xffff);
}

Lexer::Lexer(std::unique_ptr<std::istream> stream) noexcept
    : utf8_stream_(std::move(stream)) {}

std::uint32_t Lexer::Next() noexcept { return 0; }

}  // namespace graphql_cpp
