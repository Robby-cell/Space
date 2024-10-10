#ifndef SPACE_STRING_HPP
#define SPACE_STRING_HPP

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
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
  using size_type = std::size_t;
  using difference_type = ptrdiff_t;

  using allocator_type = Alty;

  using iterator = void;
  using const_iterator = void;
  // NOLINTEND

  struct StringAlloc {
    Char* ptr;
    std::size_t capacity;
  };
  static constexpr std::size_t MaxBufSize{(sizeof(StringAlloc) / sizeof(Char)) -
                                          1};
  union Storage {
    Char buf[MaxBufSize] = {};
    StringAlloc alloc;
    Char alias[MaxBufSize];
  };

  constexpr auto swap(BasicString& other) noexcept -> void {
    using std::swap;

    std::swap(compressed.storage, other.compressed.storage);
    std::swap(compressed.length, other.compressed.length);
    std::swap(compressed.is_small, other.compressed.is_small);
  }

 private:
  auto DeallocateCurrentString() -> void {
    if (!compressed.is_small) {
      AltyTraits::deallocate(compressed, compressed.storage.alloc.ptr,
                             compressed.storage.alloc.capacity);
    }
  }

  auto TakeCStr(const Char* const str) -> void {
    compressed.length = Detail::Strlen(str);
    TakeCStr(str, compressed.length);
  }

  auto TakeCStr(const Char* const str, const std::size_t len) -> void {
    if (len > MaxBufSize) {
      compressed.storage.alloc.capacity = len + 1;
      compressed.storage.alloc.ptr =
          AltyTraits::allocate(compressed, compressed.storage.alloc.capacity);

      std::copy(str, str + len, compressed.storage.alloc.ptr);

      compressed.storage.alloc.ptr[len] = Char(0);
      compressed.is_small = false;
    } else {
      std::copy(str, str + len, compressed.storage.buf);

      compressed.storage.buf[len] = Char(0);
      compressed.is_small = true;
    }
  }

 public:
  constexpr BasicString() noexcept : compressed() {}
  constexpr explicit BasicString(const Char* const str)
      : compressed{.storage = Storage()} {
    TakeCStr(str);
  }
  constexpr BasicString(const Char* const str, const std::size_t len)
      : compressed{.storage = Storage()} {
    TakeCStr(str, len);
  }

  constexpr BasicString(const BasicString& other)
      :  compressed{
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
    if (compressed.is_small) {
      return compressed.storage.buf;
    } else {
      return compressed.storage.alloc.ptr;
    }
  }
  constexpr auto data() noexcept -> Char* {
    if (compressed.is_small) {
      return compressed.storage.buf;
    } else {
      return compressed.storage.alloc.ptr;
    }
  }
  constexpr auto length() const noexcept -> size_type {
    return compressed.length;
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
    Storage storage = {};
    std::size_t length = 0;
    bool is_small = true;
  };
  Compressed compressed;

  constexpr auto allocator() noexcept -> Alty { return compressed; };
};

using String = BasicString<char, std::allocator<char>>;
using WString = BasicString<wchar_t, std::allocator<wchar_t>>;

}  // namespace Space

#endif  // SPACE_STRING_HPP
