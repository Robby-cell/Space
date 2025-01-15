#ifndef SPACE_STRING_VIEW_HPP
#define SPACE_STRING_VIEW_HPP

#include <cstddef>
#include <iterator>

#include "space/detail.hpp"

namespace Space {

template <class StringType>
struct StringViewIterator {
 public:
  // NOLINTBEGIN
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename StringType::value_type;
  using pointer = typename StringType::const_pointer;
  using reference = const value_type&;
  using difference_type = typename StringType::difference_type;
  // NOLINTEND

  constexpr explicit StringViewIterator(value_type* ptr) noexcept : ptr_(ptr) {}

  constexpr auto operator++() noexcept -> StringViewIterator& {
    ++ptr_;
    return *this;
  }
  constexpr auto operator++(int) noexcept -> StringViewIterator {
    auto tmp = *this;
    this->operator++();
    return tmp;
  }
  constexpr auto operator--() noexcept -> StringViewIterator& {
    --ptr_;
    return *this;
  }
  constexpr auto operator--(int) noexcept -> StringViewIterator {
    auto tmp = *this;
    this->operator--();
    return tmp;
  }

  constexpr auto operator+=(const difference_type distance) noexcept
      -> StringViewIterator& {
    ptr_ += distance;
    return *this;
  }
  constexpr auto operator-=(const difference_type distance) noexcept
      -> StringViewIterator& {
    ptr_ -= distance;
    return *this;
  }

  constexpr auto operator+(const difference_type distance) const noexcept
      -> StringViewIterator {
    auto tmp{*this};
    tmp += distance;
    return tmp;
  }
  constexpr auto operator-(const difference_type distance) const noexcept
      -> StringViewIterator {
    auto tmp{*this};
    tmp -= distance;
    return tmp;
  }

  constexpr auto operator[](const difference_type distance) const noexcept
      -> reference {
    return ptr_[distance];
  }

  constexpr auto operator*() const noexcept -> reference { return *ptr_; }

#if SPACE_HAS_CXX20
  constexpr auto operator<=>(const StringConstIterator& other) const noexcept
      -> std::strong_ordering = default;
#else   // ^^^ SPACE_HAS_CXX20
  constexpr auto operator==(const StringViewIterator& other) const noexcept
      -> bool {
    return ptr_ == other.ptr_;
  }
  constexpr auto operator!=(const StringViewIterator& other) const noexcept
      -> bool {
    return !(ptr_ == other.ptr_);
  }
  constexpr auto operator<(const StringViewIterator& other) const noexcept
      -> bool {
    return ptr_ < other.ptr_;
  }
  constexpr auto operator>(const StringViewIterator& other) const noexcept
      -> bool {
    return other < *this;
  }
  constexpr auto operator<=(const StringViewIterator& other) const noexcept
      -> bool {
    return !(other < *this);
  }
  constexpr auto operator>=(const StringViewIterator& other) const noexcept
      -> bool {
    return !(*this < other);
  }
#endif  // ^^^ !SPACE_HAS_CXX20

  constexpr auto advance(const difference_type distance) noexcept -> void {
    ptr_ += distance;
  }

 protected:
  value_type* ptr_;
};

template <typename Char>
struct StringView {
 public:
  // NOLINTBEGIN
  using value_type = Char;
  using pointer = const Char*;
  using reference = const Char&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = StringViewIterator<StringView>;
  // NOLINTEND

  constexpr StringView(const Char* ptr, size_type length)
      : ptr_(ptr), length_(length) {}

  constexpr StringView(const Char* ptr)  // NOLINT
      : StringView(ptr, Detail::Strlen(ptr)) {}

  constexpr StringView(const auto& container)  // NOLINT
    requires requires {
      container.data();
      container.size();
    }
      : StringView(container.data(), container.size()) {}

  constexpr auto length() const noexcept -> size_type { return length_; }
  constexpr auto data() const noexcept -> pointer { return ptr_; }

  constexpr auto begin() const noexcept -> iterator { return iterator(ptr_); }
  constexpr auto end() const noexcept -> iterator {
    return iterator(ptr_ + length_);
  }

  friend constexpr auto operator==(const StringView& self,
                                   const StringView& that) {
    return (self.length() == that.length()) &&
           Detail::StrNeq(self.data(), that.data(), self.length());
  }

  friend constexpr auto operator!=(const StringView& self, auto&& that)
    requires requires {
      that.data();
      that.size();
    }
  {
    return !(self == StringView(that.data(), that.size()));
  }

  friend constexpr auto operator!=(const StringView& self, auto&& that) {
    return !(self == that);
  }

  friend constexpr auto operator!=(auto&& self, const StringView& that) {
    return that != self;
  }

  friend constexpr auto operator==(const StringView& self, const Char* that) {
    StringView view(that);
    return self == view;
  }

 private:
  pointer ptr_;
  size_type length_;
};

}  // namespace Space

#endif  // SPACE_STRING_VIEW_HPP
