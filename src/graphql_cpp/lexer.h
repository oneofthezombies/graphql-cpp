#ifndef GRAPHQL_CPP_LEXER_H_
#define GRAPHQL_CPP_LEXER_H_

#include <cstdint>
#include <istream>
#include <memory>

#include "graphql_cpp/utf8_stream.h"

namespace graphql_cpp {

enum class TokenKind : std::uint8_t {
  EOF_TOKEN,
  BANG,
  DOLLAR,
  PAREN_L,
  PAREN_R,
  SPREAD,
};

// Reference: https://spec.graphql.org/October2021/#sec-Language.Source-Text
bool IsSourceCharacter(std::uint32_t code_point) noexcept;

class Lexer {
 public:
  explicit Lexer(std::unique_ptr<std::istream> stream) noexcept;

  ~Lexer() noexcept = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) noexcept = default;
  Lexer& operator=(Lexer&&) noexcept = default;

  [[nodiscard]] std::uint32_t Next() noexcept;

 private:
  Utf8Stream utf8_stream_;
  std::size_t line_ = 1;
  std::size_t column_ = 1;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_LEXER_H_
