#include "day06.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 06") {

  SUBCASE("Part 1") {
    Day06 solution{};
    solution.set_input("./data/06/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    Day06 solution{};
    solution.set_input("./data/06/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day06 solution{};
    solution.set_input("./data/06/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}