#include "day17.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 17") {

  SUBCASE("Part 1, example") {
    Day17 solution{};
    solution.set_input("./data/17/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 102);
  }

  SUBCASE("Part 2, example") {
    Day17 solution{};
    solution.set_input("./data/17/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day17 solution{};
    solution.set_input("./data/17/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 785);
    REQUIRE_THROWS(solution.part2());
  }
}