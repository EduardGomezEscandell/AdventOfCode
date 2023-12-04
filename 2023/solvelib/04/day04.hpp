#pragma once

#include "xmaslib/solution/solution.hpp"

class Day04 : public xmas::solution {
public:
  int day() override { return 4; }

public:
  std::int64_t part1() override;
  std::int64_t part2() override;

};
