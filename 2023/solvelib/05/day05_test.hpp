#include "day05.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 05") {

  SUBCASE("Part 1, example 1") {
    Day05 solution{};
    solution.set_input("./data/05/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 82);
  }

  SUBCASE("Part 1, example 2") {
    Day05 solution{};
    solution.set_input("./data/05/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 35);
  }

  SUBCASE("Part 2") {
    Day05 solution{};
    solution.set_input("./data/05/example2.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day05 solution{};
    solution.set_input("./data/05/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 331445006);
    REQUIRE_THROWS(solution.part2());
  }
}