#pragma once

#include <concepts>
#include <format>
#include <map>
#include <memory>

#include "../solution/solution.hpp"

namespace xmas {

std::map<int, std::unique_ptr<solution>> const &registered_solutions() noexcept;

namespace internal {
std::map<int, std::unique_ptr<solution>> &registered_solutions() noexcept;
}

template <std::derived_from<xmas::solution> S> void register_solution() {
  auto &m = internal::registered_solutions();
  S s{};
  const auto day = s.day();

  if (auto x = m.find(day); x != m.end()) {
    throw std::runtime_error(std::format(
        "Attempting the overwrite registered solution for day {}", day));
  }

  m[day] = std::unique_ptr<solution>(new S(std::move(s)));
}

} // namespace xmas
