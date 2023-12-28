#include "day10.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 10") {

  SUBCASE("Part 1, example 1") {
    Day10 solution{};
    solution.set_input("./data/10/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 4);
  }

  SUBCASE("Part 1, example 2") {
    Day10 solution{};
    solution.set_input("./data/10/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 8);
  }

  SUBCASE("Part 2, example 1") {
    Day10 solution{};
    solution.set_input("./data/10/example1.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day10 solution{};
    solution.set_input("./data/10/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 6956);
    REQUIRE_THROWS(solution.part2());
  }
}