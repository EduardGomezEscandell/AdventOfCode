#include "log.hpp"
#include <iostream>

namespace xlog {
namespace internal {

void log(std::string_view prefix, std::string_view message) {
  std::cerr << std::format("{} {}\n", prefix, message) << std::flush;
}

} // namespace internal

} // namespace xlog