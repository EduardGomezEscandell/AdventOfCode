#include "day14.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 14") {
  SUBCASE("Part 1, example") {
    Day14 solution{};
    solution.set_input("./data/14/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2, example") {
    Day14 solution{};
    solution.set_input("./data/14/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day14 solution{};
    solution.set_input("./data/14/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}