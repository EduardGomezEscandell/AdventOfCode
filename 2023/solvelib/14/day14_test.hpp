#include "day14.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 14") {
  SUBCASE("Part 1, example") {
    Day14 solution{};
    solution.set_input("./data/14/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 136);
  }

  SUBCASE("Part 2, example") {
    Day14 solution{};
    solution.set_input("./data/14/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 64);
  }

  SUBCASE("Real data") {
    Day14 solution{};
    solution.set_input("./data/14/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 111979);
    REQUIRE_EQ(solution.part2(), 102055);
  }
}