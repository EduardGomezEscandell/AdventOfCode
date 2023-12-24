#pragma once

#include "xmaslib/solution/solution.hpp"

class Day22 : public xmas::solution {
public:
  int day() override {
    return 22;
  }

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
