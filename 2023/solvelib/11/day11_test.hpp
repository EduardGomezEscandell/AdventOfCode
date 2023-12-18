#include "day11.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 11") {

  SUBCASE("Part 1") {
    Day11 solution{};
    solution.set_input("./data/11/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 374);
  }

  SUBCASE("Part 2, size 10") {
    Day11 solution{};
    solution.set_input("./data/11/example.txt");
    solution.load();
    REQUIRE_EQ(solution.solve(10), 1030);
  }

  SUBCASE("Part 2, size 100") {
    Day11 solution{};
    solution.set_input("./data/11/example.txt");
    solution.load();
    REQUIRE_EQ(solution.solve(100), 8410);
  }

  SUBCASE("Real data") {
    Day11 solution{};
    solution.set_input("./data/11/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 9974721);
    REQUIRE_EQ(solution.part2(), 702770569197);
  }
}