#include "day15.hpp"
#include <execution>
#include <functional>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string_view>

void Day15::load() {
  xmas::solution::load();

  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }
}

std::uint64_t Day15::part1() {
  auto view = std::string_view(input.begin(), input.end() - 1) |
              std::ranges::views::split(',');

  return std::transform_reduce(
      std::execution::par_unseq, view.begin(), view.end(), std::uint64_t{0},
      std::plus<std::uint64_t>{},
      [](auto substring) { return hash256(substring.begin(), substring.end()); });
}

std::uint64_t Day15::part2() { throw std::runtime_error("Not implemented"); }