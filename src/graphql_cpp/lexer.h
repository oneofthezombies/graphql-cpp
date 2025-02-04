#ifndef GRAPHQL_CPP_LEXER_H_
#define GRAPHQL_CPP_LEXER_H_

#include <expected>

#include "graphql_cpp/source.h"

namespace graphql_cpp {

enum class TokenKind {
  kUnexpected,
  kEof,
};

template <CharIterator Iterator>
class Lexer {
 public:
  explicit Lexer(Source<Iterator> source) noexcept : source_(source) {}

  ~Lexer() noexcept = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) noexcept = default;
  Lexer& operator=(Lexer&&) noexcept = default;

  std::expected<TokenKind, ErrorCode> Next() noexcept {}

 private:
  Source<Iterator> source_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_LEXER_H_
