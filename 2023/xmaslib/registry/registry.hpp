#pragma once

#include <concepts>
#include <format>
#include <map>
#include <memory>

#include "../solvebase/solvebase.hpp"

namespace xmas {

std::map<int, std::unique_ptr<solution>> const &registered_solutions() noexcept;

namespace internal {
std::map<int, std::unique_ptr<solution>> &registered_solutions() noexcept;
}

template <int V, std::derived_from<xmas::solution> T> void register_solution() {
  auto &m = internal::registered_solutions();

  if (auto x = m.find(V); x != m.begin()) {
    throw std::runtime_error(std::format(
        "Attempting the overwrite registered solution for day {}", V));
  }

  m[V] = std::unique_ptr<solution>(new T());
}

} // namespace xmas