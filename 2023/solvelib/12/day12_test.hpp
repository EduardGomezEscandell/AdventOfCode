#include "day12.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 12") {

  SUBCASE("Part 1") {
    Day12 solution{};
    solution.set_input("./data/12/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    Day12 solution{};
    solution.set_input("./data/12/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day12 solution{};
    solution.set_input("./data/12/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}