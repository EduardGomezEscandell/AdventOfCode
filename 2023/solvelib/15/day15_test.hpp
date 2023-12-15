#include "day15.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 3") {

  SUBCASE("Part 1, example 1") {
    Day15 solution{};
    solution.set_input("./data/15/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2, example 1") {
    Day15 solution{};
    solution.set_input("./data/15/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day15 solution{};
    solution.set_input("./data/15/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}