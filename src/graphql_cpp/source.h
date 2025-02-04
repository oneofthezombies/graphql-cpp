#ifndef GRAPHQL_CPP_SOURCE_H_
#define GRAPHQL_CPP_SOURCE_H_

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

enum class ErrorCode {
  kUnexpected,
  kInvalidSource,
};

template <CharIterator Iterator>
class Source {
 public:
  explicit Source(Iterator begin, Iterator end) noexcept
      : it_(begin), end_(end) {}

  ~Source() noexcept = default;

  Source(const Source&) = delete;
  Source& operator=(const Source&) = delete;

  Source(Source&&) noexcept = default;
  Source& operator=(Source&&) noexcept = default;

  std::optional<char8_t> PeekByte() const noexcept {
    if (it_ == end_) {
      return std::nullopt;
    }

    return static_cast<char8_t>(*it_);
  }

  std::optional<char8_t> NextByte() noexcept {
    auto byte = PeekByte();
    if (byte) {
      Advance();
    }

    return byte;
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
  void Advance() noexcept {
    ++it_;
    ++position_;
  }

  Iterator it_;
  Iterator end_;
  std::size_t position_{};
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SOURCE_H_
