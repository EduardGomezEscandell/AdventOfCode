#include "day23.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 23") {

  SUBCASE("Part 1, example") {
    Day23 solution{};
    solution.set_input("./data/23/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 94);
  }

  SUBCASE("Part 1, test") {
    Day23 solution{};
    solution.set_input("./data/23/test.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 37);
  }

  SUBCASE("Part 2, example") {
    Day23 solution{};
    solution.set_input("./data/23/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 154);
  }

  SUBCASE("Part 2, test") {
    Day23 solution{};
    solution.set_input("./data/23/test.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 37);
  }

  SUBCASE("Real data") {
    Day23 solution{};
    solution.set_input("./data/23/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2178);
    REQUIRE_EQ(solution.part2(), 6486);
  }
}