#ifndef GRAPHQL_CPP_UTF8_BYTES_STREAM_H_
#define GRAPHQL_CPP_UTF8_BYTES_STREAM_H_

#include <expected>
#include <istream>
#include <memory>

#include "graphql_cpp/result.h"

namespace graphql_cpp {

enum class ReadByteErrorKind {
  Bad,
  Fail,
  Unexpected,
};

[[nodiscard]] std::expected<std::optional<std::uint8_t>, ReadByteErrorKind>
ReadByte(std::istream& stream) noexcept;

struct ReadByteError {
  ReadByteErrorKind kind{};
  std::size_t position{};
};

struct InvalidUtf8FirstByteError {
  std::size_t position{};
  std::uint8_t first_byte{};
};

struct InvalidUtf8ContinuationByteError {
  std::size_t position{};
  std::uint8_t continuation_byte{};
};

class Utf8BytesStream {
 public:
  struct Value {
    std::uint8_t bytes[4]{};
    std::size_t length{};
  };

  using Error = std::variant<ReadByteError, InvalidUtf8FirstByteError,
                             InvalidUtf8ContinuationByteError>;

  explicit Utf8BytesStream(std::unique_ptr<std::istream> stream) noexcept;

  ~Utf8BytesStream() noexcept = default;

  Utf8BytesStream(const Utf8BytesStream&) = delete;
  Utf8BytesStream& operator=(const Utf8BytesStream&) = delete;

  Utf8BytesStream(Utf8BytesStream&&) noexcept = default;
  Utf8BytesStream& operator=(Utf8BytesStream&&) noexcept = default;

  [[nodiscard]] Result<std::optional<Value>, Error> Next() noexcept;

 private:
  std::unique_ptr<std::istream> stream_;
  std::size_t position_{};
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_BYTES_STREAM_H_
