#pragma once

#include <format>

namespace xlog {

enum severity {
  ERROR,
  WARNING,
  INFO,
  DEBUG,
};

namespace internal {

void log(severity prefix, std::string_view message);
} // namespace internal

template <typename... Args>
constexpr void error(std::format_string<Args...> msg, Args &&...args) {
  internal::log(ERROR, std::format(msg, std::forward<Args>(args)...));
}

template <typename... Args>
constexpr void warning(std::format_string<Args...> msg, Args &&...args) {
  internal::log(WARNING, std::format(msg, std::forward<Args>(args)...));
}

template <typename... Args>
constexpr void info(std::format_string<Args...> msg, Args &&...args) {
  internal::log(INFO, std::format(msg, std::forward<Args>(args)...));
}

template <typename... Args>
constexpr void debug([[maybe_unused]] std::format_string<Args...> msg,
                     [[maybe_unused]] Args &&...args) {
#ifndef NDEBUG
  internal::log(DEBUG, std::format(msg, std::forward<Args>(args)...));
#endif
}

} // namespace xlog