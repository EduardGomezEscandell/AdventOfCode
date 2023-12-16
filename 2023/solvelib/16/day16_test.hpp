#include "day16.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 16") {

  SUBCASE("Part 1, example") {
    Day16 solution{};
    solution.set_input("./data/16/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 46);
  }

  SUBCASE("Part 2, example") {
    Day16 solution{};
    solution.set_input("./data/16/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 51);
  }

  SUBCASE("Real data") {
    Day16 solution{};
    solution.set_input("./data/16/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 7472);
    REQUIRE_EQ(solution.part2(), 7716);
  }
}