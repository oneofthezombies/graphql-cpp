#ifndef GRAPHQL_CPP_SOURCE_TEXT_H_
#define GRAPHQL_CPP_SOURCE_TEXT_H_

#include <array>
#include <expected>
#include <iterator>
#include <optional>

namespace graphql_cpp {

template <typename Iterator>
concept CharIterator =
    std::same_as<typename std::iterator_traits<Iterator>::value_type, char>;

constexpr char8_t kUtf8ContinuationByteMask = 0b11000000;
constexpr char8_t kUtf8ContinuationBytePattern = 0b10000000;
constexpr char8_t kUtf8Overlong3ByteFirst = 0xe0;
constexpr char8_t kUtf8Overlong3ByteSecondMax = 0x9f;
constexpr char8_t kUtf8Utf16SurrogateFirst = 0xed;
constexpr char8_t kUtf8Utf16SurrogateSecondMin = 0xa0;
constexpr char8_t kUtf8MaxSourceCharacterFirst = 0xef;
constexpr char8_t kUtf8MaxSourceCharacterSecond = 0xbf;
constexpr char8_t kUtf8MaxSourceCharacterThird = 0xbf;

// https://spec.graphql.org/October2021/#sec-Language.Source-Text
constexpr std::array<std::uint8_t, 256> kUtf8SourceCharacterLength = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,  // 0x00 ~ 0x0f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10 ~ 0x1f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x20 ~ 0x2f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x30 ~ 0x3f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40 ~ 0x4f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x50 ~ 0x5f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60 ~ 0x6f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x70 ~ 0x7f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x80 ~ 0x8f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x90 ~ 0x9f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xa0 ~ 0xaf
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xb0 ~ 0xbf
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xc0 ~ 0xcf
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xd0 ~ 0xdf
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 0xe0 ~ 0xef
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xf0 ~ 0xff
};

constexpr std::uint8_t Utf8SourceCharacterLength(const char8_t b0) noexcept {
  return kUtf8SourceCharacterLength[b0];
}

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

enum class ErrorCode {
  kUnexpected,
  kInvalidSource,
};

template <CharIterator CharIterator>
class SourceText {
 public:
  explicit SourceText(CharIterator begin, CharIterator end) noexcept
      : it_(begin), end_(end) {}

  ~SourceText() noexcept = default;

  SourceText(const SourceText&) = delete;
  SourceText& operator=(const SourceText&) = delete;

  SourceText(SourceText&&) noexcept = default;
  SourceText& operator=(SourceText&&) noexcept = default;

  std::optional<char8_t> PeekByte() const noexcept {
    if (it_ == end_) {
      return std::nullopt;
    }

    return static_cast<char8_t>(*it_);
  }

  void Advance() noexcept {
    ++it_;
    ++position_;
  }

  std::expected<std::optional<char32_t>, ErrorCode> NextSourceChar() noexcept {
    if (it_ == end_) {
      return std::nullopt;
    }

    const char8_t b0 = static_cast<char8_t>(*it_);
    Advance();
    const std::uint8_t len = Utf8SourceCharacterLength(b0);
    switch (len) {
      case 0:
        return std::unexpected(ErrorCode::kInvalidSource);
      case 1:
        return static_cast<char32_t>(b0);
      case 2: {
        if (it_ == end_) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        const char8_t b1 = static_cast<char8_t>(*it_);
        if (!IsUtf8ContinuationByte(b1)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        Advance();
        return Utf8ToUtf32Unchecked(b0, b1);
      }
      case 3: {
        if (it_ == end_) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        const char8_t b1 = static_cast<char8_t>(*it_);
        if (IsUtf8Overlong3Byte(b0, b1) || IsUtf8Utf16Surrogate(b0, b1) ||
            !IsUtf8ContinuationByte(b1)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        Advance();
        if (it_ == end_) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        const char8_t b2 = static_cast<char8_t>(*it_);
        if (!IsUtf8WithinMaxSourceCharacter(b0, b1, b2) ||
            !IsUtf8ContinuationByte(b2)) {
          return std::unexpected(ErrorCode::kInvalidSource);
        }

        Advance();
        return Utf8ToUtf32Unchecked(b0, b1, b2);
      }
      default:
        return std::unexpected(ErrorCode::kUnexpected);
    }
  }

 private:
  CharIterator it_;
  CharIterator end_;
  std::size_t position_{};
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SOURCE_TEXT_H_
