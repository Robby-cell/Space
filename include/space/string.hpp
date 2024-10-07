#ifndef SPACE_STRING_HPP
#define SPACE_STRING_HPP

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>

namespace std {
template <class>
struct char_traits;
template <class, class>
struct basic_ostream;
}  // namespace std

namespace Space {
namespace Detail {
template <typename Char>
constexpr auto Strlen(const Char* const str) noexcept -> size_t {
  if (str) [[likely]] {
    size_t length = 0;
    while (str[length] != Char(0)) {
      ++length;
    }
    return length;
  } else [[unlikely]] {
    return 0;
  }
}
template <typename Char, size_t Size>
constexpr auto Strlen([[maybe_unused]] const Char (&str)[Size]) noexcept
    -> size_t {
  return Size - 1;
}
template <typename Char>
constexpr auto Strcmp(const Char* const str1, const Char* const str2) noexcept {
  ptrdiff_t difference{0};
  size_t index{0};
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
constexpr auto Streq(const Char* const str1,
                     const Char* const str2) noexcept -> bool {
  return Strcmp(str1, str2) == 0;
}
}  // namespace Detail

template <typename Char, typename MyAlty = std::allocator<Char>>
class BasicString {
 private:
  // NOLINTBEGIN
  using Alty =
      typename std::allocator_traits<MyAlty>::template rebind_alloc<Char>;
  using AltyTraits = std::allocator_traits<Alty>;

 public:
  using value_type = Char;
  using reference = Char&;
  using const_reference = const Char&;
  using pointer = Char*;
  using const_pointer = const Char*;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using allocator_type = Alty;

  using iterator = void;
  using const_iterator = void;
  // NOLINTEND

  struct StringAlloc {
    Char* ptr;
    size_t capacity;
  };
  static constexpr size_t MaxBufSize{(sizeof(StringAlloc) / sizeof(Char)) - 1};
  union Storage {
    Char buf[MaxBufSize] = {};
    StringAlloc alloc;
    Char alias[MaxBufSize];
  };

  constexpr auto swap(BasicString& other) noexcept -> void {
    using std::swap;

    std::swap(storage, other.storage);
    std::swap(length, other.length);
    std::swap(is_small, other.is_small);
  }

 private:
  inline auto DeallocateCurrentString() -> void {
    if (!is_small) {
      AltyTraits::deallocate(allocator, storage.alloc.ptr,
                             storage.alloc.capacity);
    }
  }

  inline auto TakeCStr(const Char* const str) -> void {
    length = Detail::Strlen(str);
    TakeCStr(str, length);
  }

  inline auto TakeCStr(const Char* const str, const size_t len) -> void {
    if (len > MaxBufSize) {
      storage.alloc.capacity = len + 1;
      storage.alloc.ptr =
          AltyTraits::allocate(allocator, storage.alloc.capacity);

      std::copy(str, str + len, storage.alloc.ptr);

      storage.alloc.ptr[len] = Char(0);
      is_small = false;
    } else {
      std::copy(str, str + len, storage.buf);

      storage.buf[len] = Char(0);
      is_small = true;
    }
  }

 public:
  constexpr BasicString() noexcept : storage() {}
  constexpr explicit BasicString(const Char* const str) : storage() {
    TakeCStr(str);
  }

  constexpr BasicString(const BasicString& other)
      : storage(), length(other.length), is_small(other.is_small) {
    TakeCStr(other.storage.alloc.ptr);
  }
  constexpr BasicString(BasicString&& other) noexcept { swap(other); }
  auto operator=(const BasicString& other) -> BasicString& {
    if (this not_eq &other) {
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
    if (is_small) {
      return storage.buf;
    } else {
      return storage.alloc.ptr;
    }
  }
  constexpr auto data() noexcept -> Char* {
    if (is_small) {
      return storage.buf;
    } else {
      return storage.alloc.ptr;
    }
  }

 private:
  friend inline auto operator<<(
      std::basic_ostream<Char, std::char_traits<Char>>& os,
      const BasicString<Char, MyAlty>& string)
      -> std::basic_ostream<Char, std::char_traits<Char>>& {
    return os.write(string.data(), string.length);
  }
  friend constexpr auto operator==(const BasicString& lhs,
                                   const BasicString& rhs) noexcept -> bool {
    return (lhs.length == rhs.length) and Streq(lhs.data(), rhs.data());
  }
  friend constexpr auto operator==(const BasicString& lhs,
                                   const Char* const rhs) noexcept -> bool {
    return Detail::Streq(lhs.data(), rhs);
  }

  Storage storage = {};
  size_t length = 0;
  bool is_small = true;
  Alty allocator;
};

using String = BasicString<char, std::allocator<char>>;
}  // namespace Space

#endif  // SPACE_STRING_HPP
