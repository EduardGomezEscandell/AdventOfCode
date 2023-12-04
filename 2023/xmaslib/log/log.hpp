#include <format>

namespace xlog {

namespace internal {
void log(std::string_view prefix, std::string_view message);
}

template <typename... Args>
constexpr void error(std::format_string<Args...> msg, Args &&...args) {
  internal::log("\x1b[31mERROR  \x1b[0m", std::format(msg, std::forward<Args>(args)...));
}

template <typename... Args>
constexpr void warning(std::format_string<Args...> msg, Args &&...args) {
  internal::log("\x1b[33mWARNING\x1b[0m", std::format(msg, std::forward<Args>(args)...));
}

template <typename... Args>
constexpr void info(std::format_string<Args...> msg, Args &&...args) {
  internal::log("INFO   ", std::format(msg, std::forward<Args>(args)...));
}

template <typename... Args>
constexpr void debug([[maybe_unused]] std::format_string<Args...> msg,
                     [[maybe_unused]] Args &&...args) {
#ifndef NDEBUG
  internal::log("\x1b[34mDEBUG  \x1b[0m", std::format(msg, std::forward<Args>(args)...));
#endif
}

} // namespace xlog