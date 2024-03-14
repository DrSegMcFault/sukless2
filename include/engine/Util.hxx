#pragma once

#include <ranges>
#include <algorithm>
#include <type_traits>

namespace util {

  template<typename T, typename V>
  bool contains(const T& a, const V& b) {
    return std::find(a.begin(), a.end(), b) != a.end();
  }

  template <typename T>
  constexpr auto range(T end) {
    return std::views::iota(T{0}, end);
  }

  template <typename T>
  constexpr auto range(T begin, T end) {
    return std::views::iota(begin, end);
  }

  template <typename T>
  concept EnumClass = std::is_enum_v<T>;

  template<EnumClass T>
  constexpr inline auto toul(T enum_value) {
    return static_cast<std::underlying_type_t<T>>(enum_value);
  }
} // namespace util
