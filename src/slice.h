#ifndef GRAPHQL_CPP_SLICE_H_
#define GRAPHQL_CPP_SLICE_H_

#include <cassert>
#include <variant>

namespace graphql_cpp {

namespace slice {

struct NullDataError {};

template <typename T>
class Slice {
 public:
  using CreateResult = std::variant<Slice<T>, NullDataError>;

  static CreateResult Create(const T* data, const std::size_t size) noexcept {
    if (data == nullptr) {
      return NullDataError{};
    }
    return Slice<T>(data, size);
  }

  ~Slice() noexcept = default;

  Slice(const Slice&) noexcept = default;
  Slice& operator=(const Slice&) noexcept = default;

  Slice(Slice&&) noexcept = default;
  Slice& operator=(Slice&&) noexcept = default;

  const T& operator[](const std::size_t index) const noexcept {
    assert(index < size_);
    return data_[index];
  }

  const T* data() const noexcept { return data_; }

  bool empty() const noexcept { return size_ == 0; }
  std::size_t size() const noexcept { return size_; }

 private:
  explicit Slice(const T* data, const std::size_t size) noexcept
      : data_(data), size_(size) {}

  const T* data_;
  std::size_t size_;
};

}  // namespace slice

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SLICE_H_
