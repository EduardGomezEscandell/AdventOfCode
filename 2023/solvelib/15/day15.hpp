#pragma once

#include "xmaslib/solution/solution.hpp"
#include <numeric>

class Day15 : public xmas::solution {
public:
  int day() override { return 15; }

public:
  // Modulo 256 comes built-in :D
  using hash256_t = std::uint8_t;

  template <typename Iterator>
  [[nodiscard]] static constexpr hash256_t hash256(Iterator begin,
                                                   Iterator end) {
    return std::accumulate(begin, end, hash256_t{0},
                           [](hash256_t acc, char ch) constexpr {
                             return 17 * (acc + hash256_t(ch));
                           });
  }

  template <typename Range>
  [[nodiscard]] static constexpr hash256_t hash256(Range const &sv) {
    return hash256(sv.begin(), sv.end());
  }

  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
