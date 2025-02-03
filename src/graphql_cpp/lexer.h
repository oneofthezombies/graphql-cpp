#ifndef GRAPHQL_CPP_LEXER_H_
#define GRAPHQL_CPP_LEXER_H_

#include <expected>
#include <iterator>
#include <optional>

namespace graphql_cpp {

constexpr char8_t kUtf8ContinuationByteMin = 0x80;
constexpr char8_t kUtf8ContinuationByteMax = 0xbf;
constexpr char8_t kUtf8ContinuationByteMask = 0b11000000;
constexpr char8_t kUtf8ContinuationBytePattern = 0b10000000;
constexpr char8_t kUtf8Overlong3ByteFirst = 0xe0;
constexpr char8_t kUtf8Overlong3ByteSecondMax = 0x9f;
constexpr char8_t kUtf8Utf16SurrogateFirst = 0xed;
constexpr char8_t kUtf8Utf16SurrogateSecondMin = 0xa0;
constexpr char8_t kUtf8MaxSourceCharacterFirst = 0xef;
constexpr char8_t kUtf8MaxSourceCharacterSecond = 0xbf;
constexpr char8_t kUtf8MaxSourceCharacterThird = 0xbf;

std::uint8_t Utf8SourceCharacterLength(const char8_t b0) noexcept;

constexpr bool IsUtf8ContinuationByte(const char8_t b) noexcept {
  return (b & kUtf8ContinuationByteMask) == kUtf8ContinuationBytePattern;
}

constexpr bool IsUtf8Overlong3Byte(const char8_t b0,
                                   const char8_t b1) noexcept {
  return b0 == kUtf8Overlong3ByteFirst && b1 <= kUtf8Overlong3ByteSecondMax;
}

constexpr bool IsUtf8Utf16Surrogate(const char8_t b0,
                                    const char8_t b1) noexcept {
  return b0 == kUtf8Utf16SurrogateFirst && b1 >= kUtf8Utf16SurrogateSecondMin;
}

constexpr bool IsUtf8WithinMaxSourceCharacter(const char8_t b0,
                                              const char8_t b1,
                                              const char8_t b2) noexcept {
  if (b0 < kUtf8MaxSourceCharacterFirst) {
    return true;
  }
  if (b0 > kUtf8MaxSourceCharacterFirst) {
    return false;
  }
  if (b1 < kUtf8MaxSourceCharacterSecond) {
    return true;
  }
  if (b1 > kUtf8MaxSourceCharacterSecond) {
    return false;
  }
  return b2 <= kUtf8MaxSourceCharacterThird;
}

constexpr char32_t Utf8ToUtf32Unchecked(const char8_t b0,
                                        const char8_t b1) noexcept {
  return (b0 & 0b00011111) << 6 | (b1 & 0b00111111);
}

constexpr char32_t Utf8ToUtf32Unchecked(const char8_t b0, const char8_t b1,
                                        const char8_t b2) noexcept {
  return (b0 & 0b00001111) << 12 | (b1 & 0b00111111) << 6 | (b2 & 0b00111111);
}

template <typename Iterator>
concept CharIterator =
    std::same_as<typename std::iterator_traits<Iterator>::value_type, char>;

enum class TokenKind {
  kUnexpected,
  kEof,
};

enum class ErrorCode {
  kUnexpected,
  kInvalidSource,
};

template <CharIterator SourceIterator>
class Lexer {
 public:
  explicit Lexer(SourceIterator begin, SourceIterator end) noexcept
      : it_(begin), end_(end) {}

  ~Lexer() noexcept = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) noexcept = default;
  Lexer& operator=(Lexer&&) noexcept = default;

  std::expected<TokenKind, ErrorCode> NextToken() noexcept {
    if (it_ == end_) {
      return TokenKind::kEof;
    }

    const char8_t b0 = static_cast<char8_t>(*it_++);
    const std::uint8_t char_len = Utf8SourceCharacterLength(b0);
    switch (char_len) {
      case 0:
        return std::unexpected(ErrorCode::kInvalidSource);
      case 1: {
        // TODO: Implement
        return TokenKind::kUnexpected;
      } break;
      case 2: {
        if (it_ == end_) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        const char8_t b1 = static_cast<char8_t>(*it_);
        if (!IsUtf8ContinuationByte(b1)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        ++it_;
        const char32_t cp = Utf8ToUtf32Unchecked(b0, b1);
        // TODO: Implement
        return TokenKind::kUnexpected;
      } break;
      case 3: {
        if (it_ == end_) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        const char8_t b1 = static_cast<char8_t>(*it_);
        if (IsUtf8Overlong3Byte(b0, b1)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        if (IsUtf8Utf16Surrogate(b0, b1)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        if (!IsUtf8ContinuationByte(b1)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        ++it_;
        if (it_ == end_) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        const char8_t b2 = static_cast<char8_t>(*it_);
        if (!IsUtf8WithinMaxSourceCharacter(b0, b1, b2)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        if (!IsUtf8ContinuationByte(b2)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        ++it_;
        const char32_t cp = Utf8ToUtf32Unchecked(b0, b1, b2);
        // TODO: Implement
        return TokenKind::kUnexpected;
      } break;
      default:
        return std::unexpected(ErrorCode::kUnexpected);
    }
  }

  std::optional<char> NextChar() noexcept {
    if (it_ == end_) {
      return std::nullopt;
    }
    return *it_++;
  }

 private:
  SourceIterator it_;
  SourceIterator end_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_LEXER_H_
