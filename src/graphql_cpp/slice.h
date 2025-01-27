#ifndef GRAPHQL_CPP_SLICE_H_
#define GRAPHQL_CPP_SLICE_H_

#include <cassert>
#include <cstddef>

namespace graphql_cpp {

template <typename T>
class Slice {
 public:
  constexpr explicit Slice(const T* data, std::size_t size) noexcept
      : data_(data), size_(size) {
    assert(data_ != nullptr);
  }

  constexpr ~Slice() noexcept = default;

  constexpr Slice(const Slice&) noexcept = default;
  constexpr Slice& operator=(const Slice&) noexcept = default;

  constexpr Slice(Slice&&) noexcept = default;
  constexpr Slice& operator=(Slice&&) noexcept = default;

  constexpr const T& operator[](std::size_t index) const& noexcept {
    assert(index < size_);
    return data_[index];
  }

  constexpr const T* Data() const noexcept { return data_; }
  constexpr bool Empty() const noexcept { return size_ == 0; }
  constexpr std::size_t Size() const noexcept { return size_; }

 private:
  const T* data_;
  std::size_t size_;
};

}  // namespace graphql_cpp

#endif  // GRAPHQL_CPP_SLICE_H_
