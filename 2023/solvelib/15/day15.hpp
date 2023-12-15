#pragma once

#include "xmaslib/solution/solution.hpp"
#include <execution>
#include <string_view>

class Day15 : public xmas::solution {
public:
  int day() override { return 15; }
  void load() override;

public:
  // Modulo 256 comes built-in :D
  using hash256_t = std::uint8_t;

  template <typename Iterator>
  static hash256_t hash256(Iterator begin, Iterator end) {
    return std::reduce(std::execution::unseq, begin, end, hash256_t{0},
                       [](hash256_t l, hash256_t r) { return 17 * (l + r); });
  }

  static hash256_t hash256(std::string_view sv) {
    return hash256(sv.begin(), sv.end());
  };

  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
