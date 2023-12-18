#include "day19.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 19") {

  SUBCASE("Part 1, example") {
    Day19 solution{};
    solution.set_input("./data/19/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 19114);
  }

  SUBCASE("Part 2, example") {
    Day19 solution{};
    solution.set_input("./data/19/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day19 solution{};
    solution.set_input("./data/19/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 402185);
    REQUIRE_THROWS(solution.part2());
  }
}