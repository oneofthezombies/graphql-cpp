#ifndef GRAPHQL_CPP_UTF8_DECODER_H_
#define GRAPHQL_CPP_UTF8_DECODER_H_

#include <variant>

#include "slice.h"

namespace graphql_cpp {

namespace utf8_decoder {

struct Eof {};

struct CodePoint {
  std::uint32_t value;
};

struct FirstByteError {
  std::size_t position;
  std::uint8_t first_byte;
};

struct CodePointLengthError {
  std::size_t position;
  std::size_t code_point_length;
  std::size_t text_length;
};

struct ContinuationByteError {
  std::size_t position;
  std::uint8_t continuation_byte;
};

using NextCodePointResult =
    std::variant<Eof, CodePoint, FirstByteError, CodePointLengthError,
                 ContinuationByteError>;

class Utf8Decoder {
 public:
  explicit Utf8Decoder(const slice::Slice<std::uint8_t> text) noexcept;

  ~Utf8Decoder() noexcept = default;

  Utf8Decoder(const Utf8Decoder&) noexcept = default;
  Utf8Decoder& operator=(const Utf8Decoder&) noexcept = default;

  Utf8Decoder(Utf8Decoder&&) noexcept = default;
  Utf8Decoder& operator=(Utf8Decoder&&) noexcept = default;

  [[nodiscard]] NextCodePointResult NextCodePoint() noexcept;

 private:
  slice::Slice<std::uint8_t> text_;
  std::size_t position_ = 0;
};

}  // namespace utf8_decoder

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_DECODER_H_
