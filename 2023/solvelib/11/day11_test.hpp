#include "day11.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 11") {

  SUBCASE("Part 1") {
    Day11 solution{};
    solution.set_input("./data/11/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    Day11 solution{};
    solution.set_input("./data/11/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day11 solution{};
    solution.set_input("./data/11/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}