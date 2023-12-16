#pragma once

#include "xmaslib/solution/solution.hpp"

class Day16 : public xmas::solution {
public:
  int day() override { return 16; }
  void load() override;

public:
  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
