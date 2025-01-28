#include "graphql_cpp/stream_reader.h"

#include "graphql_cpp/error_code.h"

namespace graphql_cpp {

StreamReader::StreamReader(std::istream& stream) noexcept
    : stream_(stream), stream_position_(stream_.tellg()) {}

StreamReader::ReadResult StreamReader::Read() noexcept {
  const std::size_t pos = stream_position_;
  char c{};
  stream_.get(c);
  if (stream_.good()) {
    stream_position_++;
    return Byte{static_cast<std::uint8_t>(c), pos};
  } else if (stream_.eof()) {
    return Eof{pos};
  } else if (stream_.bad()) {
    return Error::Builder()
        .Code(ErrorCode::kStreamBad)
        .Message("Stream state is bad.")
        .Build();
  } else if (stream_.fail()) {
    return Error::Builder()
        .Code(ErrorCode::kStreamFail)
        .Message("Stream state is fail.")
        .Build();
  } else {
    return Error::Builder()
        .Code(ErrorCode::kUnexpected)
        .Message("An unexpected error occurred.")
        .Build();
  }
}

std::size_t StreamReader::Position() const noexcept { return stream_position_; }

bool IsByte(const StreamReader::Item& item) noexcept {
  return std::holds_alternative<Byte>(item);
}

bool IsEof(const StreamReader::Item& item) noexcept {
  return std::holds_alternative<Eof>(item);
}

Byte GetByte(const StreamReader::Item& item) noexcept {
  return std::get<Byte>(item);
}

Byte GetByte(StreamReader::Item&& item) noexcept {
  return std::get<Byte>(std::move(item));
}

Eof GetEof(const StreamReader::Item& item) noexcept {
  return std::get<Eof>(item);
}

Eof GetEof(StreamReader::Item&& item) noexcept {
  return std::get<Eof>(std::move(item));
}

Byte operator*(const StreamReader::Item& item) noexcept {
  return GetByte(item);
}

Byte operator*(StreamReader::Item&& item) noexcept {
  return GetByte(std::move(item));
}

}  // namespace graphql_cpp
