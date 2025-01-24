#ifndef GRAPHQL_CPP_SLICE_H_
#define GRAPHQL_CPP_SLICE_H_

#include <cassert>
#include <cstddef>

namespace graphql_cpp {

template <typename T>
class Slice {
 public:
  explicit Slice(const T* data, const std::size_t size) noexcept
      : data_(data), size_(size) {
    if (size_ > 0) {
      assert(data_ != nullptr);
    }
  }

  ~Slice() noexcept = default;

  Slice(const Slice&) = default;
  Slice& operator=(const Slice&) = default;

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
  const T* data_;
  std::size_t size_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SLICE_H_
