#ifndef GRAPHQL_CPP_LANGUAGE_H_
#define GRAPHQL_CPP_LANGUAGE_H_

#include <variant>

#include "slice.h"
#include "utf8_decoder.h"

namespace graphql_cpp {

class SourceText {
 public:
  explicit SourceText(const Slice<std::uint8_t> bytes) noexcept;
  ~SourceText() noexcept = default;

  SourceText(const SourceText&) = delete;
  SourceText& operator=(const SourceText&) = delete;

  SourceText(SourceText&&) noexcept = default;
  SourceText& operator=(SourceText&&) noexcept = default;

  struct NextSourceCharacter {
    using Eof = Utf8Decoder::NextCodePoint::Eof;
    struct SourceCharacter {
      std::uint32_t code_point;
    };
    struct NonSourceCharacter {
      std::uint32_t code_point;
    };
    using FirstByteError = Utf8Decoder::NextCodePoint::FirstByteError;
    using OutOfRangeError = Utf8Decoder::NextCodePoint::OutOfRangeError;
    using ContinuationByteError =
        Utf8Decoder::NextCodePoint::ContinuationByteError;
    using Result =
        std::variant<Eof, SourceCharacter, NonSourceCharacter, FirstByteError,
                     OutOfRangeError, ContinuationByteError>;
  };
  // Reference: https://spec.graphql.org/October2021/#sec-Language.Source-Text
  [[nodiscard]] NextSourceCharacter::Result NextSourceCharacter() noexcept;

 private:
  Utf8Decoder utf8_decoder_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_LANGUAGE_H_
