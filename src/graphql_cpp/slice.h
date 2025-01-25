#ifndef GRAPHQL_CPP_SLICE_H_
#define GRAPHQL_CPP_SLICE_H_

#include <cassert>
#include <cstddef>

namespace graphql_cpp {

/**
 * A non-owning view of contiguous elements.
 *
 * Manages a pointer and size without owning the underlying data.
 */
template <typename T>
class Slice {
 public:
  explicit Slice(const T* data, std::size_t size) noexcept
      : data_(data), size_(size) {
    assert(data_ != nullptr);
  }

  ~Slice() noexcept = default;

  Slice(const Slice&) noexcept = default;
  Slice& operator=(const Slice&) noexcept = default;

  Slice(Slice&&) noexcept = default;
  Slice& operator=(Slice&&) noexcept = default;

  constexpr const T& operator[](std::size_t index) const& noexcept {
    assert(index < size_);
    return data_[index];
  }

  constexpr const T* data() const noexcept { return data_; }

  constexpr bool empty() const noexcept { return size_ == 0; }

  constexpr std::size_t size() const noexcept { return size_; }

 private:
  const T* data_;
  std::size_t size_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SLICE_H_
