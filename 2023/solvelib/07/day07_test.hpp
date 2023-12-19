#include "day07.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 07") {

  SUBCASE("Part 1") {
    Day07 solution{};
    solution.set_input("./data/07/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 6440);
  }

  SUBCASE("Part 2") {
    Day07 solution{};
    solution.set_input("./data/07/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 5905);
  }

  SUBCASE("Real data") {
    Day07 solution{};
    solution.set_input("./data/07/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 246424613);
    REQUIRE_EQ(solution.part2(), 248256639);
  }
}