#include "log.hpp"
#include <cassert>
#include <iostream>

namespace xlog {

logger& logger::global() {
  static logger global{};
  return global;
}

void logger::set_severity(severity s) {
  this->min_severity = s;
}

void logger::log_impl(severity sever, std::string_view message) {
  const auto p = [sever]() -> std::string_view {
    switch (sever) {
    case ERROR:
      return "\x1b[31mERROR  \x1b[0m";
    case WARNING:
      return "\x1b[33mWARNING\x1b[0m";
    case INFO:
      return "INFO   ";
    case DEBUG:
      return "\x1b[34mDEBUG  \x1b[0m";
    }
    assert(false);
    return "\x1b[31mUNKNOWN SEVERITY\x1b[0m";
  }();

  std::cerr << std::format("{} {}\n", p, message) << std::flush;
}

} // namespace xlog