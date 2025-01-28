#ifndef GRAPHQL_CPP_ERROR_H_
#define GRAPHQL_CPP_ERROR_H_

#include <source_location>
#include <sstream>

#include "graphql_cpp/utils.h"

namespace graphql_cpp {

class Error {
 public:
  using SelfType = Error;
  using DetailsType =
      std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>;

  class Builder {
   public:
    Builder() noexcept = default;
    ~Builder() noexcept = default;

    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;

    Builder(Builder&&) noexcept = default;
    Builder& operator=(Builder&&) noexcept = default;

    template <Int32Convertible CodeType>
    [[nodiscard]] constexpr Builder& Code(CodeType code) noexcept {
      code_ = static_cast<std::int32_t>(code);
      return *this;
    }

    template <>
    [[nodiscard]] constexpr Builder& Code(std::int32_t code) noexcept {
      code_ = code;
      return *this;
    }

    [[nodiscard]] constexpr Builder& Message(std::string&& message) noexcept {
      message_ = std::move(message);
      return *this;
    }

    template <typename Value>
    [[nodiscard]] Builder& Detail(std::string&& key, Value&& value) noexcept {
      CheckDuplicateDetailKey(key);
      std::ostringstream oss;
      oss << std::forward<Value>(value);
      details_.emplace(std::move(key), oss.str());
      return *this;
    }

    template <>
    [[nodiscard]] constexpr Builder& Detail(std::string&& key,
                                            std::string&& value) noexcept {
      CheckDuplicateDetailKey(key);
      details_.emplace(std::move(key), std::move(value));
      return *this;
    }

    [[nodiscard]] Builder& Cause(std::unique_ptr<SelfType> cause) noexcept;

    [[nodiscard]] SelfType Build(std::source_location location =
                                     std::source_location::current()) noexcept;

   private:
    void CheckDuplicateDetailKey(std::string_view key) const noexcept;

    std::int32_t code_{kDefaultCode};
    std::string message_{kDefaultMessage};
    DetailsType details_;
    std::unique_ptr<SelfType> cause_;
  };

  static constexpr std::int32_t kDefaultCode = 1;
  static constexpr std::string_view kDefaultMessage =
      "An unexpected error occurred.";

  Error(std::int32_t code, std::string&& message, DetailsType&& details,
        std::unique_ptr<SelfType> cause,
        std::source_location&& location) noexcept;
  ~Error() noexcept = default;

  Error(const SelfType&) = delete;
  SelfType& operator=(const SelfType&) = delete;

  Error(SelfType&&) noexcept = default;
  SelfType& operator=(SelfType&&) noexcept = default;

  constexpr std::int32_t Code() const noexcept { return code_; }
  constexpr std::string_view Message() const noexcept { return message_; }
  constexpr const DetailsType& Details() const noexcept { return details_; }

  [[nodiscard]] std::optional<std::string_view> Detail(
      std::string&& key) const noexcept;

  constexpr const std::unique_ptr<SelfType>& Cause() const noexcept {
    return cause_;
  }
  constexpr const std::source_location& Location() const noexcept {
    return location_;
  }

  [[nodiscard]] std::unique_ptr<SelfType> ReleaseAsUnique() noexcept;

  [[nodiscard]] operator std::unique_ptr<SelfType>() noexcept;

 private:
  std::int32_t code_;
  std::string message_;
  DetailsType details_;
  std::unique_ptr<SelfType> cause_;
  std::source_location location_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_ERROR_H_
