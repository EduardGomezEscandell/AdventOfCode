#pragma once

#include <format>
#include <string_view>
#include <utility>

namespace xlog {

enum severity {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
};

struct logger {
  severity min_severity = DEBUG;

  static logger& global();
  void set_severity(severity s);

  template <typename... Args>
  void log(severity severity, std::format_string<Args...> msg, Args&&... args) {
    if (severity < min_severity) {
      return;
    }
    return log_impl(severity, std::format(msg, std::forward<Args>(args)...));
  }

  template <typename... Args>
  constexpr void error(std::format_string<Args...> msg, Args&&... args) {
    log(ERROR, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  constexpr void warning(std::format_string<Args...> msg, Args&&... args) {
    log(WARNING, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  constexpr void info(std::format_string<Args...> msg, Args&&... args) {
    log(INFO, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  constexpr void debug([[maybe_unused]] std::format_string<Args...> msg,
    [[maybe_unused]] Args&&... args) {
#ifndef NDEBUG
    log(DEBUG, msg, std::forward<Args>(args)...);
#endif
  }

private:
  void log_impl(severity prefix, std::string_view message);
};

template <typename... Args>
constexpr void error(std::format_string<Args...> msg, Args&&... args) {
  logger::global().error(msg, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void warning(std::format_string<Args...> msg, Args&&... args) {
  logger::global().warning(msg, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void info(std::format_string<Args...> msg, Args&&... args) {
  logger::global().info(msg, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void debug([[maybe_unused]] std::format_string<Args...> msg,
  [[maybe_unused]] Args&&... args) {
#ifndef NDEBUG
  logger::global().debug(msg, std::forward<Args>(args)...);
#endif
}

} // namespace xlog