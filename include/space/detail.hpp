#ifndef SPACE_DETAIL_HPP
#define SPACE_DETAIL_HPP

#include <cstddef>

namespace Space::Detail {
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
constexpr auto Strcmp(const Char* const str1, const Char* const str2) noexcept
    -> std::ptrdiff_t {
  std::ptrdiff_t difference{0};
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
  return difference;
}

template <typename Char>
constexpr auto Streq(const Char* const str1, const Char* const str2) noexcept {
  return Strcmp(str1, str2) == 0;
}

template <typename Char>
constexpr auto StrNcmp(const Char* const str1, const Char* const str2,
                       std::size_t len) noexcept -> std::ptrdiff_t {
  std::ptrdiff_t difference{0};
  std::size_t index{0};
  while (len--) {
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
  return difference;
}

template <typename Char>
constexpr auto StrNeq(const Char* const str1, const Char* const str2,
                      std::size_t const len) noexcept {
  return Strcmp(str1, str2, len) == 0;
}
}  // namespace Space::Detail

#endif  // SPACE_DETAIL_HPP
