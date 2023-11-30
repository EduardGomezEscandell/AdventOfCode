#include "registry.hpp"

namespace xmas {

std::map<int, std::unique_ptr<solution>> solutions{};

std::map<int, std::unique_ptr<solution>> const &registered_solutions() noexcept {
  return solutions;
}

namespace internal {

std::map<int, std::unique_ptr<solution>> &registered_solutions() noexcept {
  return solutions;
}

} // namespace internal

} // namespace xmas