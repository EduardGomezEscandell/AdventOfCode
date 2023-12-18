#include "day18.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 18") {

  SUBCASE("Part 1, example") {
    Day18 solution{};
    solution.set_input("./data/18/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 62);
  }

  SUBCASE("Part 2, example") {
    Day18 solution{};
    solution.set_input("./data/18/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 952408144115);
  }

  SUBCASE("Real data") {
    Day18 solution{};
    solution.set_input("./data/18/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 48400);
    REQUIRE_EQ(solution.part2(), 72811019847283);
  }
}