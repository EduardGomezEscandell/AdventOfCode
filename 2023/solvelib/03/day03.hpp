#pragma once

#include "xmaslib/solution/solution.hpp"

class Day03 : public xmas::solution {
public:
  int day() override { return 3; }
  void load() override;

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
