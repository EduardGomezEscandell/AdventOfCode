#pragma once

#include "xmaslib/solution/solution.hpp"

class Day18 : public xmas::solution {
public:
  int day() override {
    return 18;
  }

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
