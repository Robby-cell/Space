#ifndef SPACE_STRING_HPP
#define SPACE_STRING_HPP

#define SPACE_HAS_CXX20 (__cplusplus >= 202002L)

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ostream>
#include <utility>
#if SPACE_HAS_CXX20
#include <compare>
#endif

namespace std {
template <class>
struct char_traits;
template <class, class>
struct basic_ostream;
}  // namespace std

namespace Space {
namespace Detail {
template <typename Char>
constexpr auto Strlen(const Char* const str) noexcept -> std::size_t {
  if (str) [[likely]] {
    std::size_t length = 0;
    while (str[length] != Char(0)) {
      ++length;
    }
    return length;
  } else [[unlikely]] {
    return 0;
  }
}
template <typename Char, std::size_t Size>
constexpr auto Strlen(const Char (&)[Size]) noexcept -> std::size_t {
  return Size - 1;
}
template <typename Char>
constexpr auto Strcmp(const Char* const str1, const Char* const str2) noexcept {
  ptrdiff_t difference{0};
  std::size_t index{0};
  while (true) {
    char c1 = str1[index];
    char c2 = str2[index];
    if (c1 != c2) {
      return c1 - c2;
    }
    if (c1 == 0) {
      return 0;
    }
    ++index;
  }
}
template <typename Char>
constexpr auto Streq(const Char* const str1, const Char* const str2) noexcept {
  return Strcmp(str1, str2) == 0;
}
}  // namespace Detail

template <class StringType>
struct StringConstIterator {
 public:
  // NOLINTBEGIN
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename StringType::value_type;
  using pointer = typename StringType::const_pointer;
  using reference = const value_type&;
  using difference_type = typename StringType::difference_type;
  // NOLINTEND

  constexpr explicit StringConstIterator(value_type* ptr) noexcept
      : ptr_(ptr) {}

  constexpr auto operator++() noexcept -> StringConstIterator& {
    ++ptr_;
    return *this;
  }
  constexpr auto operator++(int) noexcept -> StringConstIterator {
    auto tmp = *this;
    this->operator++();
    return tmp;
  }
  constexpr auto operator--() noexcept -> StringConstIterator& {
    --ptr_;
    return *this;
  }
  constexpr auto operator--(int) noexcept -> StringConstIterator {
    auto tmp = *this;
    this->operator--();
    return tmp;
  }

  constexpr auto operator+=(const difference_type distance) noexcept
      -> StringConstIterator& {
    ptr_ += distance;
    return *this;
  }
  constexpr auto operator-=(const difference_type distance) noexcept
      -> StringConstIterator& {
    ptr_ -= distance;
    return *this;
  }

  constexpr auto operator+(const difference_type distance) const noexcept
      -> StringConstIterator {
    auto tmp{*this};
    tmp += distance;
    return tmp;
  }
  constexpr auto operator-(const difference_type distance) const noexcept
      -> StringConstIterator {
    auto tmp{*this};
    tmp += distance;
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
  constexpr auto operator==(const StringConstIterator& other) const noexcept
      -> bool {
    return ptr_ == other.ptr_;
  }
  constexpr auto operator!=(const StringConstIterator& other) const noexcept
      -> bool {
    return ptr_ != other.ptr_;
  }
  constexpr auto operator<(const StringConstIterator& other) const noexcept
      -> bool {
    return ptr_ < other.ptr_;
  }
  constexpr auto operator>(const StringConstIterator& other) const noexcept
      -> bool {
    return other < *this;
  }
  constexpr auto operator<=(const StringConstIterator& other) const noexcept
      -> bool {
    return !(other < *this);
  }
  constexpr auto operator>=(const StringConstIterator& other) const noexcept
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
template <class StringType>
struct StringIterator : public StringConstIterator<StringType> {
 public:
  using Base = StringConstIterator<StringType>;
  // NOLINTBEGIN
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename StringType::value_type;
  using pointer = typename StringType::pointer;
  using reference = value_type&;
  using difference_type = typename StringType::difference_type;
  // NOLINTEND

  constexpr explicit StringIterator(value_type* ptr) noexcept : Base(ptr) {}

  constexpr auto operator++() noexcept {
    Base::operator++();
    return *this;
  }
  constexpr auto operator++(int) noexcept {
    auto tmp = *this;
    this->operator++();
    return tmp;
  }
  constexpr auto operator--() noexcept {
    Base::operator--();
    return *this;
  }
  constexpr auto operator--(int) noexcept {
    auto tmp = *this;
    this->operator--();
    return tmp;
  }

  constexpr auto operator+=(const difference_type distance) noexcept
      -> StringIterator& {
    Base::operator+=(distance);
    return *this;
  }
  constexpr auto operator-=(const difference_type distance) noexcept
      -> StringIterator& {
    Base::operator-=(distance);
    return *this;
  }
  constexpr auto operator+(const difference_type distance) const noexcept
      -> StringIterator {
    auto tmp{*this};
    tmp += distance;
    return tmp;
  }
  constexpr auto operator-(const difference_type distance) const noexcept
      -> StringIterator {
    auto tmp{*this};
    tmp += distance;
    return tmp;
  }

  constexpr auto operator[](const difference_type distance) const noexcept
      -> reference {
    return const_cast<reference>(Base::operator[](distance));
  }

  constexpr auto operator*() const noexcept -> reference { return *Base::ptr_; }

 private:
};

template <typename Char, typename MyAlty = std::allocator<Char>>
class BasicString {
 private:
  // NOLINTBEGIN
  using Self = BasicString<Char, MyAlty>;

  using Alty =
      typename std::allocator_traits<MyAlty>::template rebind_alloc<Char>;
  using AltyTraits = std::allocator_traits<Alty>;

 public:
  using value_type = Char;
  using reference = Char&;
  using const_reference = const Char&;
  using pointer = Char*;
  using const_pointer = const Char*;
  using size_type = std::size_t;
  using difference_type = ptrdiff_t;

  using allocator_type = Alty;

  using iterator = StringIterator<Self>;
  using const_iterator = StringConstIterator<Self>;
  // NOLINTEND

  struct StringAlloc {
    Char* ptr;
    std::size_t capacity;
  };
  static constexpr std::size_t MaxBufSize{(sizeof(StringAlloc) / sizeof(Char))};
  static constexpr std::size_t MaxBufLength{MaxBufSize - 1};
  union Storage {
    constexpr Storage() : buf{} {}

    Char buf[MaxBufSize] = {};
    StringAlloc alloc;
    Char alias[MaxBufSize];
  };

  constexpr auto swap(BasicString& other) noexcept -> void {
    using std::swap;

    std::swap(compressed_.storage, other.compressed_.storage);
    std::swap(compressed_.length, other.compressed_.length);
    std::swap(compressed_.is_small, other.compressed_.is_small);
  }

 private:
  auto DeallocateCurrentString() -> void {
    if (!compressed_.is_small) {
      AltyTraits::deallocate(compressed_, compressed_.storage.alloc.ptr,
                             compressed_.storage.alloc.capacity);
    }
  }

  auto TakeCStr(const Char* const str) -> void {
    compressed_.length = Detail::Strlen(str);
    TakeCStr(str, compressed_.length);
  }

  auto TakeCStr(const Char* const str, const std::size_t len) -> void {
    if (len > MaxBufLength) {
      compressed_.storage.alloc.capacity = len + 1;
      compressed_.storage.alloc.ptr =
          AltyTraits::allocate(compressed_, compressed_.storage.alloc.capacity);

      std::copy(str, str + len, compressed_.storage.alloc.ptr);

      compressed_.storage.alloc.ptr[len] = Char(0);
      compressed_.is_small = false;
    } else {
      std::copy(str, str + len, compressed_.storage.buf);

      compressed_.storage.buf[len] = Char(0);
      compressed_.is_small = true;
    }
  }

 public:
  constexpr BasicString() noexcept = default;
  constexpr explicit BasicString(const Char* const str)
      : compressed_{.storage = Storage()} {
    TakeCStr(str);
  }
  constexpr BasicString(const Char* const str, const std::size_t len)
      : compressed_{.storage = Storage()} {
    TakeCStr(str, len);
  }

  constexpr BasicString(const BasicString& other)
      :  compressed_{
        .storage = Storage(),
        .length = other.length,
        .is_small = other.is_small,
      }
      // storage(), length(other.length), is_small(other.is_small)
       {
    TakeCStr(other.storage.alloc.ptr);
  }
  constexpr BasicString(BasicString&& other) noexcept { swap(other); }
  auto operator=(const BasicString& other) -> BasicString& {
    if (this != &other) {
      DeallocateCurrentString();
      TakeCStr(other.data(), other.length);
    }
    return *this;
  }
  constexpr auto operator=(BasicString&& other) noexcept -> BasicString& {
    swap(other);
    return *this;
  }

  constexpr auto operator=(const Char* const str) noexcept -> BasicString& {
    DeallocateCurrentString();
    TakeCStr(str);
    return *this;
  }

  ~BasicString() { DeallocateCurrentString(); }

  constexpr auto data() const noexcept -> const Char* {
    if (compressed_.is_small) {
      return compressed_.storage.buf;
    } else {
      return compressed_.storage.alloc.ptr;
    }
  }
  constexpr auto data() noexcept -> Char* {
    if (compressed_.is_small) {
      return compressed_.storage.buf;
    } else {
      return compressed_.storage.alloc.ptr;
    }
  }
  constexpr auto length() const noexcept -> size_type {
    return compressed_.length;
  }

 private:
  friend auto operator<<(std::ostream& os, const BasicString<Char, MyAlty>& str)
      -> std::ostream& {
    return os.write(str.data(), str.length);
  }
  friend constexpr auto operator==(const BasicString& lhs,
                                   const BasicString& rhs) noexcept -> bool {
    return (lhs.length() == rhs.length()) && Streq(lhs.data(), rhs.data());
  }
  friend constexpr auto operator==(const BasicString& lhs,
                                   const Char* const rhs) noexcept -> bool {
    return Detail::Streq(lhs.data(), rhs);
  }

  struct Compressed : public Alty {
    Storage storage = Storage();
    std::size_t length = 0;
    bool is_small = true;
  };
  Compressed compressed_ = Compressed();

  constexpr auto allocator() noexcept -> Alty { return compressed_; };
};

using String = BasicString<char, std::allocator<char>>;
using WString = BasicString<wchar_t, std::allocator<wchar_t>>;

}  // namespace Space

#endif  // SPACE_STRING_HPP
