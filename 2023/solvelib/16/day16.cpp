#include "day16.hpp"

#include <stdexcept>

void Day16::load() {
  xmas::solution::load();

  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }
}

std::uint64_t Day16::part1() { throw std::runtime_error("Not implemented"); }

std::uint64_t Day16::part2() { throw std::runtime_error("Not implemented"); }