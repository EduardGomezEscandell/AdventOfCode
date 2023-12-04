#include "day04.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 04") {

  SUBCASE("Part 1, example 1") {
    Day04 solution{};
    solution.set_input("./data/04/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2);
  }

  SUBCASE("Part 1, example 2") {
    Day04 solution{};
    solution.set_input("./data/04/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 13);
  }

  SUBCASE("Part 2") {
    Day04 solution{};
    solution.set_input("./data/04/example3.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 30);
  }

  SUBCASE("Real data") {
    Day04 solution{};
    solution.set_input("./data/04/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 23028);
    REQUIRE_EQ(solution.part2(), 9236992);
  }
}