#include "day06.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 06") {

  SUBCASE("Part 1") {
    Day06 solution{};
    solution.set_input("./data/06/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 288);
  }

  SUBCASE("Part 2") {
    Day06 solution{};
    solution.set_input("./data/06/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 71503);
  }

  SUBCASE("Real data") {
    Day06 solution{};
    solution.set_input("./data/06/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 588588);
    REQUIRE_EQ(solution.part2(), 34655848);
  }
}