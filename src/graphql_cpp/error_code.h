#ifndef GRAPHQL_CPP_ERROR_CODE_H_
#define GRAPHQL_CPP_ERROR_CODE_H_

#include <cstdint>

namespace graphql_cpp {

enum class ErrorCode : std::int32_t {
  kUnexpected = 1,

  // StreamReader
  kStreamBad = 2,
  kStreamFail = 3,

  // UTF-8
  kUtf8InvalidFirstByte = 4,
  kUtf8IncompleteBytes = 5,
  kUtf8InvalidContinuationByte = 6,
  kUtf8OverlongOf3Bytes = 7,
  kUtf8UnicodeSurrogate = 8,
  kUtf8OverlongOf4Bytes = 9,
  kUtf8BeyondUnicodeRange = 10,
};

constexpr std::int32_t ToInt32(ErrorCode code) noexcept {
  return static_cast<std::int32_t>(code);
}

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_ERROR_CODE_H_
