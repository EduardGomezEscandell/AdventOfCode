#include <format>
#include <iostream>

namespace xlog {

template <typename... Args>
constexpr void error(std::format_string<Args...> msg, Args &&...args) {
  std::cerr << "\x1b[31mERROR  \x1b[0m "
            << std::format(msg, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
constexpr void warning(std::format_string<Args...> msg, Args &&...args) {
  std::cerr << "\x1b[33mWARNING\x1b[0m "
            << std::format(msg, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
constexpr void info(std::format_string<Args...> msg, Args &&...args) {
  std::cerr << "INFO   " << std::format(msg, std::forward<Args &&>(args)...)
            << std::endl;
}

template <typename... Args>
constexpr void debug([[maybe_unused]] std::format_string<Args...> msg,
                     [[maybe_unused]] Args &&...args) {
#ifndef NDEBUG
  std::cerr << "\x1b[34mDEBUG  \x1b[0m "
            << std::format(msg, std::forward<Args &&>(args)...) << std::endl;
#endif
}

} // namespace xlog