#pragma once

#include <format>
#include <string>
#include <type_traits>
#include <utility>

namespace xmas {

template <typename T>
  requires(std::is_invocable<T>::value)
struct lazy_string {
  T generate;

  constexpr lazy_string(T&& generate) : generate(std::forward<T>(generate)) {
  }
};

} // namespace xmas

template <typename T>
struct std::formatter<xmas::lazy_string<T>> : std::formatter<std::string> {
  auto format(xmas::lazy_string<T> const& s, format_context& ctx) const {
    return formatter<std::string>::format(s.generate(), ctx);
  }
};