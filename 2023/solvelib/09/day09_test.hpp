#include "day09.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 09") {

  SUBCASE("Part 1, example") {
    Day09 solution{};
    solution.set_input("./data/09/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2, example") {
    Day09 solution{};
    solution.set_input("./data/09/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day09 solution{};
    solution.set_input("./data/09/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}