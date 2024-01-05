#pragma once

#include "xmaslib/solution/solution.hpp"

class Day24 : public xmas::solution {
public:
  int day() override {
    return 24;
  }

public:
  std::uint64_t part1_generalized(std::int64_t min, std::int64_t max);

  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
