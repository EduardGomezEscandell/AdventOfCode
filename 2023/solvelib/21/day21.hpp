#pragma once

#include "xmaslib/solution/solution.hpp"

class Day21 : public xmas::solution {
public:
  int day() override {
    return 21;
  }

public:
  std::uint64_t part1_generalized(std::size_t nsteps);
  std::uint64_t part2_generalized(std::size_t nsteps);

  std::uint64_t part1() override;
  std::uint64_t part2() override;
};
