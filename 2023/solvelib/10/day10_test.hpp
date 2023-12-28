#include "day10.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 10") {

  SUBCASE("Part 1, example 1") {
    Day10 solution{};
    solution.set_input("./data/10/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 4);
  }

  SUBCASE("Part 1, example 2") {
    Day10 solution{};
    solution.set_input("./data/10/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 8);
  }

  SUBCASE("Part 2, example 1") {
    Day10 solution{};
    solution.set_input("./data/10/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 1);
  }

  SUBCASE("Part 2, example 2") {
    Day10 solution{};
    solution.set_input("./data/10/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 1);
  }

  SUBCASE("Part 2, example 3") {
    Day10 solution{};
    solution.set_input("./data/10/example3.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 4);
  }

  SUBCASE("Part 2, example 4") {
    Day10 solution{};
    solution.set_input("./data/10/example4.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 10);
  }

  SUBCASE("Real data") {
    Day10 solution{};
    solution.set_input("./data/10/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 6956);
    REQUIRE_EQ(solution.part2(), 455);
  }
}