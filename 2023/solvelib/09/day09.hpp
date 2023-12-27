#pragma once

#include "xmaslib/solution/solution.hpp"

class Day09 : public xmas::solution {
public:
  int day() override {
    return 9;
  }

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
