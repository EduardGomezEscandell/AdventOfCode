#include "day24.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 24") {

  SUBCASE("Part 1, example") {
    Day24 solution{};
    solution.set_input("./data/24/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1_generalized(7, 27), 2);
  }

  SUBCASE("Part 2, example") {
    Day24 solution{};
    solution.set_input("./data/24/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 47);
  }

  SUBCASE("Real data") {
    Day24 solution{};
    solution.set_input("./data/24/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 27732);
    REQUIRE_EQ(solution.part2(), 641619849766168);
  }
}