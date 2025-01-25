#ifndef GRAPHQL_CPP_UTF8_STREAM_H_
#define GRAPHQL_CPP_UTF8_STREAM_H_

#include <istream>
#include <memory>

#include "graphql_cpp/utils.h"

namespace graphql_cpp {

struct BadStreamError {
  std::size_t position;
};

struct FailStreamError {
  std::size_t position;
};

struct UnexpectedStreamError {
  std::size_t position;
};

struct Utf8FirstByteError {
  std::size_t position;
  std::uint8_t first_byte;
};

struct Utf8CodePointLengthError {
  std::size_t position;
  std::size_t code_point_length;
};

struct Utf8ContinuationByteError {
  std::size_t position;
  std::uint8_t continuation_byte;
};

std::optional<std::size_t> Utf8CodePointLength(
    std::uint8_t first_byte) noexcept;
std::optional<std::uint32_t> Utf8CodePoint(std::uint8_t* bytes,
                                           std::size_t length) noexcept;
bool IsUtf8ContinuationByte(std::uint8_t byte) noexcept;

class Utf8Stream {
 public:
  using NextError =
      std::variant<BadStreamError, FailStreamError, UnexpectedStreamError,
                   Utf8FirstByteError, Utf8CodePointLengthError,
                   Utf8ContinuationByteError>;

  explicit Utf8Stream(std::unique_ptr<std::istream> stream) noexcept;

  ~Utf8Stream() noexcept = default;

  Utf8Stream(const Utf8Stream&) = delete;
  Utf8Stream& operator=(const Utf8Stream&) = delete;

  Utf8Stream(Utf8Stream&&) noexcept = default;
  Utf8Stream& operator=(Utf8Stream&&) noexcept = default;

  [[nodiscard]] Result<std::optional<std::uint32_t>, NextError> Next() noexcept;

 private:
  using ReadByteError =
      std::variant<BadStreamError, FailStreamError, UnexpectedStreamError>;

  [[nodiscard]] Result<std::optional<std::uint8_t>, ReadByteError>
  ReadByte() noexcept;

  [[nodiscard]] Result<std::optional<std::uint32_t>, NextError> NextCodePoint(
      std::uint8_t first_byte, std::size_t code_point_length) noexcept;

  std::unique_ptr<std::istream> stream_;
  std::size_t position_ = 0;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_STREAM_H_
