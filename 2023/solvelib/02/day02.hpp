#pragma once

#include "xmaslib/solution/solution.hpp"

class Day02 : public xmas::solution {
public:
  int day() override { return 2; }

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;

};
