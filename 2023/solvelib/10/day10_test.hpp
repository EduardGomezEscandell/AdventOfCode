#include "day10.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 10") {

  SUBCASE("Part 1, example") {
    Day10 solution{};
    solution.set_input("./data/10/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2, example") {
    Day10 solution{};
    solution.set_input("./data/10/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day10 solution{};
    solution.set_input("./data/10/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}