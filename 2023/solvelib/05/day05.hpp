#pragma once

#include "xmaslib/solution/solution.hpp"

class Day05 : public xmas::solution {
public:
  int day() override { return 5; }

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
