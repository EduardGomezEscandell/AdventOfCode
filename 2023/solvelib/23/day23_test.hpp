#include "day23.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 23") {

  SUBCASE("Part 1, example") {
    Day23 solution{};
    solution.set_input("./data/23/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 94);
  }

  SUBCASE("Part 2, example") {
    Day23 solution{};
    solution.set_input("./data/23/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day23 solution{};
    solution.set_input("./data/23/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2178);
    REQUIRE_THROWS(solution.part2());
  }
}