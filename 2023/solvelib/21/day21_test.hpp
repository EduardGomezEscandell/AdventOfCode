#include "day21.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 21") {

  SUBCASE("Part 1, example 1") {
    Day21 solution{};
    solution.set_input("./data/21/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 1, example 2") {
    Day21 solution{};
    solution.set_input("./data/21/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day21 solution{};
    solution.set_input("./data/21/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}