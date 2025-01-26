#ifndef GRAPHQL_CPP_UTF8_STREAM_READER_H_
#define GRAPHQL_CPP_UTF8_STREAM_READER_H_

#include <istream>

#include "graphql_cpp/any_error.h"
#include "graphql_cpp/result.h"

namespace graphql_cpp {

struct Utf8StreamReader {
  using ReadResult = Result<std::optional<std::uint32_t>, AnyError>;

  explicit Utf8StreamReader(std::istream& stream) noexcept;

  ~Utf8StreamReader() noexcept = default;

  Utf8StreamReader(const Utf8StreamReader&) = delete;
  Utf8StreamReader& operator=(const Utf8StreamReader&) = delete;

  Utf8StreamReader(Utf8StreamReader&&) = delete;
  Utf8StreamReader& operator=(Utf8StreamReader&&) = delete;

  [[nodiscard]] ReadResult read() noexcept;

 private:
  [[nodiscard]] ReadResult parse_first_byte(std::uint8_t first_byte) noexcept;
  [[nodiscard]] ReadResult parse_continuation_bytes_length(
      std::uint8_t first_byte, std::size_t continuation_bytes_length) noexcept;
  [[nodiscard]] ReadResult parse_overlong_encoding(
      std::uint32_t code_point, std::size_t utf8_bytes_length) noexcept;
  [[nodiscard]] ReadResult parse_surrogate_pair(
      std::uint32_t code_point, std::size_t utf8_bytes_length) noexcept;
  [[nodiscard]] ReadResult parse_code_point_beyond_unicode_range(
      std::uint32_t code_point, std::size_t utf8_bytes_length) noexcept;
  [[nodiscard]] ReadResult bad_error() noexcept;
  [[nodiscard]] ReadResult fail_error() noexcept;
  [[nodiscard]] ReadResult unexpected_error() noexcept;

  std::istream& stream_;
  std::size_t position_{0};
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_STREAM_READER_H_
