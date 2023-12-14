#include "log.hpp"
#include <cassert>
#include <iostream>

namespace xlog {
namespace internal {

void log(severity prefix, std::string_view message) {
  const auto p = [prefix]() -> std::string_view {
    switch (prefix) {
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

  std::cerr << std::format("{}: {}\n", p, message) << std::flush;
}

} // namespace internal

} // namespace xlog