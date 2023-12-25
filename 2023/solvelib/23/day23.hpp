#pragma once

#include "xmaslib/solution/solution.hpp"

class Day23 : public xmas::solution {
public:
  int day() override {
    return 23;
  }

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
