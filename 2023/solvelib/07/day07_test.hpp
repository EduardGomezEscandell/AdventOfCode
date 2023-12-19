#include "day07.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 07") {

  SUBCASE("Part 1") {
    Day07 solution{};
    solution.set_input("./data/07/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2") {
    Day07 solution{};
    solution.set_input("./data/07/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day07 solution{};
    solution.set_input("./data/07/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}