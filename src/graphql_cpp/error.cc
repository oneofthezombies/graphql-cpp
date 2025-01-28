#include "graphql_cpp/error.h"

#include <cassert>

namespace graphql_cpp {

Error::Builder& Error::Builder::Cause(
    std::unique_ptr<SelfType> cause) noexcept {
  cause_ = std::move(cause);
  return *this;
}

Error Error::Builder::Build(std::source_location location) noexcept {
  return Error(code_, std::move(message_), std::move(details_),
               std::move(cause_), std::move(location));
}

void Error::Builder::CheckDuplicateDetailKey(
    std::string_view key) const noexcept {
  assert(details_.find(key) == details_.end());
}

Error::Error(std::int32_t code, std::string&& message, DetailsType&& details,
             std::unique_ptr<SelfType> cause,
             std::source_location&& location) noexcept
    : code_(code),
      message_(std::move(message)),
      details_(std::move(details)),
      cause_(std::move(cause)),
      location_(std::move(location)) {}

std::optional<std::string_view> Error::Detail(
    std::string&& key) const noexcept {
  auto&& it = details_.find(std::move(key));
  if (it == details_.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::unique_ptr<Error> Error::ReleaseAsUnique() noexcept {
  return std::make_unique<Error>(std::move(*this));
}

Error::operator std::unique_ptr<SelfType>() noexcept {
  return ReleaseAsUnique();
}

}  // namespace graphql_cpp
