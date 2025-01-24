#ifndef GRAPHQL_CPP_UTF8_DECODER_H_
#define GRAPHQL_CPP_UTF8_DECODER_H_

#include <optional>
#include <variant>

#include "slice.h"

namespace graphql_cpp {

// Reference: https://en.wikipedia.org/wiki/UTF-8
std::optional<std::size_t> Utf8BytesLength(
    const std::uint8_t first_byte) noexcept;

class Utf8Decoder {
 public:
  explicit Utf8Decoder(const Slice<std::uint8_t> text) noexcept;
  ~Utf8Decoder() noexcept = default;

  Utf8Decoder(const Utf8Decoder&) = default;
  Utf8Decoder& operator=(const Utf8Decoder&) = default;

  Utf8Decoder(Utf8Decoder&&) noexcept = default;
  Utf8Decoder& operator=(Utf8Decoder&&) noexcept = default;

  struct NextCodePoint {
    struct Eof {};
    struct CodePoint {
      std::uint32_t value;
    };
    struct FirstByteError {
      std::size_t position;
    };
    struct OutOfRangeError {
      std::size_t position;
      std::size_t utf8_bytes_length;
      std::size_t text_length;
    };
    struct ContinuationByteError {
      std::size_t position;
    };
    using Result = std::variant<Eof, CodePoint, FirstByteError, OutOfRangeError,
                                ContinuationByteError>;
  };
  [[nodiscard]] NextCodePoint::Result NextCodePoint() noexcept;

 private:
  Slice<std::uint8_t> text_;
  std::size_t position_ = 0;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_DECODER_H_
