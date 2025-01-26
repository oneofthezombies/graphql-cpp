#ifndef GRAPHQL_CPP_LEXER_H_
#define GRAPHQL_CPP_LEXER_H_

#include <istream>

#include "graphql_cpp/utf8_stream_reader.h"

namespace graphql_cpp {

enum class TokenKind : std::uint8_t {
  EOF_TOKEN,
  BANG,
  DOLLAR,
  PAREN_L,
  PAREN_R,
  SPREAD,
};

struct Lexer {
  using NextResult = Result<TokenKind, AnyError>;

  explicit Lexer(std::istream& stream) noexcept;

  ~Lexer() noexcept = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) = delete;
  Lexer& operator=(Lexer&&) = delete;

  [[nodiscard]] NextResult next() noexcept;

 private:
  Utf8StreamReader reader_;
  std::size_t line_ = 1;
  std::size_t column_ = 1;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_LEXER_H_
