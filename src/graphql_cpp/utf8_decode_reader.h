#ifndef GRAPHQL_CPP_UTF8_DECODE_READER_H_
#define GRAPHQL_CPP_UTF8_DECODE_READER_H_

#include <istream>
#include <variant>

#include "graphql_cpp/error_base.h"
#include "graphql_cpp/result.h"

namespace graphql_cpp {

using CodePoint = std::uint32_t;

class Utf8DecodeReader {
 public:
  enum class ErrorCode {
    kUnexpected,
    kBadStreamState,
    kFailStreamState,
    kUnexpectedStreamState,
    kInvalidUtf8FirstByte,
    k3ByteCodePointOverlong,
    k4ByteCodePointOverlong,
    kIncompleteUtf8Bytes,
    kInvalidUtf8ContinuationByte,
    kUnicodeSurrogate,
    kBeyondUnicodeRange,
  };

  struct Byte {
    std::uint8_t value{};
    std::size_t position{};
  };
  struct Eof {
    std::size_t position{};
  };

  using Error = ErrorBase<ErrorCode, ErrorCode::kUnexpected>;
  using ReadResult = Result<std::optional<CodePoint>, Error>;

  Utf8DecodeReader(std::istream& stream) noexcept;
  ~Utf8DecodeReader() = default;

  Utf8DecodeReader(const Utf8DecodeReader&) = delete;
  Utf8DecodeReader& operator=(const Utf8DecodeReader&) = delete;
  Utf8DecodeReader(Utf8DecodeReader&&) = delete;
  Utf8DecodeReader& operator=(Utf8DecodeReader&&) = delete;

  ReadResult Read() noexcept;

 private:
  using ReadByteResult = Result<std::variant<Byte, Eof>, Error>;

  ReadByteResult ReadByte() noexcept;
  ReadResult ReadSecondByte(std::size_t bytes_length, Byte b0) noexcept;
  ReadResult ReadThirdByte(std::size_t bytes_length, Byte b0, Byte b1) noexcept;
  ReadResult ReadFourthByte(Byte b0, Byte b1, Byte b2) noexcept;

  std::istream& stream_;
  std::size_t stream_position_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_UTF8_DECODE_READER_H_
