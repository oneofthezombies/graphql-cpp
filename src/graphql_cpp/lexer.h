#ifndef GRAPHQL_CPP_LEXER_H_
#define GRAPHQL_CPP_LEXER_H_

#include <expected>

#include "graphql_cpp/source_text.h"

namespace graphql_cpp {

enum class TokenKind {
  kUnexpected,
  kEof,
};

template <CharIterator CharIterator>
class Lexer {
 public:
  explicit Lexer(SourceText<CharIterator> source_text) noexcept
      : source_text_(source_text) {}

  ~Lexer() noexcept = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) noexcept = default;
  Lexer& operator=(Lexer&&) noexcept = default;

  std::expected<TokenKind, ErrorCode> Next() noexcept {
    while (true) {
      auto b0 = source_text_.PeekByte();
      if (!b0) {
        return TokenKind::kEof;
      }

      switch (*b0) {
        // https://spec.graphql.org/October2021/#sec-White-Space
        case '\t':
        case ' ':
        case ',':  // https://spec.graphql.org/October2021/#sec-Insignificant-Commas
        {
          ++column_;
          source_text_.Advance();
          continue;
        }
        // https://spec.graphql.org/October2021/#sec-Line-Terminators
        case '\n':
        case '\r': {
          ++line_;
          column_ = 1;
          source_text_.Advance();
          if (*b0 == '\r') {
            auto b1 = source_text_.PeekByte();
            if (b1 && *b1 == '\n') {
              source_text_.Advance();
            }
          }
          continue;
        }
        // https://spec.graphql.org/October2021/#sec-Comments
        case '#': {
          ++column_;
          source_text_.Advance();
          // TODO:
          // https://spec.graphql.org/October2021/#sec-Context-Free-Grammar
          continue;
        }
        default:
          break;
      }
    }
  }

 private:
  SourceText<CharIterator> source_text_;
  std::size_t line_{1};
  std::size_t column_{1};
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_LEXER_H_
