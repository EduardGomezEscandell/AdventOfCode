#include "day20.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 20") {

  SUBCASE("Part 1, example 1") {
    Day20 solution{};
    solution.set_input("./data/20/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 32000000);
  }

  SUBCASE("Part 1, example 2") {
    Day20 solution{};
    solution.set_input("./data/20/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 11687500);
  }

  SUBCASE("Real data") {
    Day20 solution{};
    solution.set_input("./data/20/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 788081152);
    REQUIRE_EQ(solution.part2(), 224602011344203);
  }
}