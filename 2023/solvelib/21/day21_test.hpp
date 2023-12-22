#include "day21.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 21") {

  SUBCASE("Part 1, example") {
    Day21 solution{};
    solution.set_input("./data/21/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1_generalized(1), 2);
    REQUIRE_EQ(solution.part1_generalized(2), 4);
    REQUIRE_EQ(solution.part1_generalized(3), 6);
    REQUIRE_EQ(solution.part1_generalized(6), 16);
  }

  SUBCASE("Part 2, example") {
    Day21 solution{};
    solution.set_input("./data/21/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day21 solution{};
    solution.set_input("./data/21/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3733);
    REQUIRE_THROWS(solution.part2());
  }
}