#include "day03.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 3") {
  Day03 solution{};
  solution.set_input("./data/03/example1.txt");
  solution.load();

  SUBCASE("Part 1") { REQUIRE_THROWS(solution.part1()); }
  SUBCASE("Part 2") { REQUIRE_THROWS(solution.part2()); }
}