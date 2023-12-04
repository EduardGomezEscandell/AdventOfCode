#include "day04.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 04") {

  SUBCASE("Part 1") {
    Day04 solution{};
    solution.set_input("./data/04/example1.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    Day04 solution{};
    solution.set_input("./data/04/example1.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }
}