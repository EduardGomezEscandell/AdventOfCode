#include "day13.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 13") {
  Day13 solution{};

  SUBCASE("Part 1") {
    solution.set_input("./data/13/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    solution.set_input("./data/13/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    solution.set_input("./data/13/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}