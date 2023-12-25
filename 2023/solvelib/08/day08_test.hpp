#include "day08.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 08") {

  SUBCASE("Part 1, example 1") {
    Day08 solution{};
    solution.set_input("./data/08/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2);
  }

  SUBCASE("Part 1, example 2") {
    Day08 solution{};
    solution.set_input("./data/08/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 6);
  }

  SUBCASE("Part 2") {
    Day08 solution{};
    solution.set_input("./data/08/example1.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day08 solution{};
    solution.set_input("./data/08/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 16579);
    REQUIRE_THROWS(solution.part2());
  }
}