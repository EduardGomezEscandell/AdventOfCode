#include "day03.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 3") {

  SUBCASE("Part 1, example 1") {
    Day03 solution{};
    solution.set_input("./data/03/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 4361);
  }

  SUBCASE("Part 1, example 2") {
    Day03 solution{};
    solution.set_input("./data/03/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 584);
  }

  SUBCASE("Part 1, example 3") {
    Day03 solution{};
    solution.set_input("./data/03/example3.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 713);
  }

  SUBCASE("Part 2, example 1") {
    Day03 solution{};
    solution.set_input("./data/03/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 467835);
  }

  SUBCASE("Part 2, example 2") {
    Day03 solution{};
    solution.set_input("./data/03/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 53238);
  }

  SUBCASE("Part 2, example 3") {
    Day03 solution{};
    solution.set_input("./data/03/example3.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 71160);
  }

  // SUBCASE("Real data") {
  //   Day03 solution{};
  //   solution.set_input("./data/03/input.txt");
  //   solution.load();
  //   REQUIRE_EQ(solution.part1(), 554003);
  //   REQUIRE_THROWS(solution.part2());
  // }
}