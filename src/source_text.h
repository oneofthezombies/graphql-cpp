#ifndef GRAPHQL_CPP_SOURCE_TEXT_H_
#define GRAPHQL_CPP_SOURCE_TEXT_H_

#include <variant>

#include "slice.h"
#include "utf8_decoder.h"

namespace graphql_cpp {

namespace source_text {

using Eof = utf8_decoder::Eof;

struct SourceCharacter {
  std::uint32_t code_point;
};

struct NonSourceCharacter {
  std::uint32_t code_point;
};

using FirstByteError = utf8_decoder::FirstByteError;
using OutOfRangeError = utf8_decoder::OutOfRangeError;
using ContinuationByteError = utf8_decoder::ContinuationByteError;

class SourceText {
 public:
  using NextSourceCharacterResult =
      std::variant<Eof, SourceCharacter, NonSourceCharacter, FirstByteError,
                   OutOfRangeError, ContinuationByteError>;

  explicit SourceText(const slice::Slice<std::uint8_t> bytes) noexcept;

  ~SourceText() noexcept = default;

  SourceText(const SourceText&) noexcept = default;
  SourceText& operator=(const SourceText&) noexcept = default;

  SourceText(SourceText&&) noexcept = default;
  SourceText& operator=(SourceText&&) noexcept = default;

  // Reference: https://spec.graphql.org/October2021/#sec-Language.Source-Text
  [[nodiscard]] NextSourceCharacterResult NextSourceCharacter() noexcept;

 private:
  utf8_decoder::Utf8Decoder utf8_decoder_;
};

}  // namespace source_text

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SOURCE_TEXT_H_
