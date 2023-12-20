#include "day20.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 20") {

  SUBCASE("Part 1, example 1") {
    Day20 solution{};
    solution.set_input("./data/20/example1.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 1, example 2") {
    Day20 solution{};
    solution.set_input("./data/20/example2.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
  }

  SUBCASE("Part 2, example 1") {
    Day20 solution{};
    solution.set_input("./data/20/example1.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day20 solution{};
    solution.set_input("./data/20/input.txt");
    solution.load();
    REQUIRE_THROWS(solution.part1());
    REQUIRE_THROWS(solution.part2());
  }
}