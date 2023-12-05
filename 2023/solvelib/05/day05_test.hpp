#include "day05.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 05") {

  SUBCASE("Part 1") {
    Day05 solution{};
    solution.set_input("./data/05/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    Day05 solution{};
    solution.set_input("./data/05/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day05 solution{};
    solution.set_input("./data/05/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}