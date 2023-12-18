#include "day18.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 18") {

  SUBCASE("Part 1, example") {
    Day18 solution{};
    solution.set_input("./data/18/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2, example") {
    Day18 solution{};
    solution.set_input("./data/18/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day18 solution{};
    solution.set_input("./data/18/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}