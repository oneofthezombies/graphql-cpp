#ifndef GRAPHQL_CPP_ERROR_H_
#define GRAPHQL_CPP_ERROR_H_

#include <cassert>
#include <memory>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "graphql_cpp/utils.h"

namespace graphql_cpp {

template <EnumClass CodeType, CodeType kDefaultCode>
class ErrorBase {
 public:
  using SelfType = ErrorBase<CodeType, kDefaultCode>;
  using DetailsType =
      std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>;

  class Builder {
   public:
    Builder() noexcept = default;
    ~Builder() noexcept = default;

    Builder(const Builder& other) = delete;
    Builder& operator=(const Builder& other) = delete;

    Builder(Builder&& other) noexcept = default;
    Builder& operator=(Builder&& other) noexcept = default;

    Builder& Code(CodeType code) noexcept {
      code_ = code;
      return *this;
    }

    Builder& Message(std::string&& message) noexcept {
      message_ = std::move(message);
      return *this;
    }

    template <typename Value>
    Builder& Detail(std::string&& key, Value&& value) noexcept {
      CheckDuplicateDetailKey(key);
      std::ostringstream oss;
      oss << std::forward<Value>(value);
      details_.emplace(std::move(key), oss.str());
      return *this;
    }

    template <>
    Builder& Detail(std::string&& key, std::string&& value) noexcept {
      CheckDuplicateDetailKey(key);
      details_.emplace(std::move(key), std::move(value));
      return *this;
    }

    template <typename Value>
    Builder& Detail(std::string_view key, Value&& value) noexcept {
      CheckDuplicateDetailKey(key);
      return Detail(std::string(key), std::forward<Value>(value));
    }

    template <>
    Builder& Detail(std::string_view key, std::string&& value) noexcept {
      CheckDuplicateDetailKey(key);
      details_.emplace(std::string(key), std::move(value));
      return *this;
    }

    Builder& Cause(std::unique_ptr<SelfType> cause) noexcept {
      cause_ = std::move(cause);
      return *this;
    }

    [[nodiscard]] SelfType Build(std::source_location location =
                                     std::source_location::current()) noexcept {
      return SelfType(code_, std::move(message_), std::move(details_),
                      std::move(cause_), std::move(location));
    }

   private:
    void CheckDuplicateDetailKey(std::string_view key) const {
      assert(details_.find(key) == details_.end());
    }

    CodeType code_{kDefaultCode};
    std::string message_{kDefaultMessage};
    DetailsType details_;
    std::unique_ptr<SelfType> cause_;
  };

  static constexpr std::string_view kDefaultMessage =
      "An unexpected error occurred.";

  ErrorBase(CodeType code, std::string&& message, DetailsType&& details,
            std::unique_ptr<SelfType> cause,
            std::source_location&& location) noexcept
      : code_(code),
        message_(std::move(message)),
        details_(std::move(details)),
        cause_(std::move(cause)),
        location_(std::move(location)) {}
  ~ErrorBase() noexcept = default;

  ErrorBase(const SelfType& other) = delete;
  SelfType& operator=(const SelfType& other) = delete;

  ErrorBase(SelfType&& other) noexcept = default;
  SelfType& operator=(SelfType&& other) noexcept = default;

  CodeType Code() const noexcept { return code_; }

  std::string_view Message() const noexcept { return message_; }

  const DetailsType& Details() const noexcept { return details_; }

  const std::unique_ptr<SelfType>& Cause() const noexcept { return cause_; }

  const std::source_location& Location() const noexcept { return location_; }

  [[nodiscard]] std::unique_ptr<SelfType> ReleaseAsUnique() noexcept {
    return std::make_unique<SelfType>(std::move(*this));
  }

  [[nodiscard]] operator std::unique_ptr<SelfType>() noexcept {
    return ReleaseAsUnique();
  }

 private:
  CodeType code_;
  std::string message_;
  DetailsType details_;
  std::unique_ptr<SelfType> cause_;
  std::source_location location_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_ERROR_H_
