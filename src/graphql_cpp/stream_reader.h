#ifndef GRAPHQL_CPP_STREAM_READER_H_
#define GRAPHQL_CPP_STREAM_READER_H_

#include <istream>

#include "graphql_cpp/error.h"
#include "graphql_cpp/result.h"

namespace graphql_cpp {

struct Byte {
  std::uint8_t value{};
  std::size_t position{};
};

struct Eof {
  std::size_t position{};
};

class StreamReader {
 public:
  using Item = std::variant<Byte, Eof>;

  using ReadResult = Result<Item, Error>;

  explicit StreamReader(std::istream& stream) noexcept;
  ~StreamReader() = default;

  StreamReader(const StreamReader&) = delete;
  StreamReader& operator=(const StreamReader&) = delete;
  StreamReader(StreamReader&&) = delete;
  StreamReader& operator=(StreamReader&&) = delete;

  ReadResult Read() noexcept;
  std::size_t Position() const noexcept;

 private:
  std::istream& stream_;
  std::size_t stream_position_;
};

bool IsByte(const StreamReader::Item& item) noexcept;
bool IsEof(const StreamReader::Item& item) noexcept;

Byte GetByte(const StreamReader::Item& item) noexcept;
Byte GetByte(StreamReader::Item&& item) noexcept;

Eof GetEof(const StreamReader::Item& item) noexcept;
Eof GetEof(StreamReader::Item&& item) noexcept;

Byte operator*(const StreamReader::Item& item) noexcept;
Byte operator*(StreamReader::Item&& item) noexcept;

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_STREAM_READER_H_
