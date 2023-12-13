#include "day12.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 12") {

  SUBCASE("Part 1, example 1") {
    Day12 solution{};
    solution.set_input("./data/12/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 4);
  }

  SUBCASE("Part 1, example 2") {
    Day12 solution{};
    solution.set_input("./data/12/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 1);
  }

  SUBCASE("Part 1, example 3") {
    Day12 solution{};
    solution.set_input("./data/12/example3.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 10);
  }

  SUBCASE("Part 1, example 4") {
    Day12 solution{};
    solution.set_input("./data/12/example4.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 1);
  }

  SUBCASE("Part 1, example 5") {
    Day12 solution{};
    solution.set_input("./data/12/example5.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 4);
  }

  SUBCASE("Part 1, example 6") {
    Day12 solution{};
    solution.set_input("./data/12/example6.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 21);
  }

  SUBCASE("Part 1, example 7") {
    Day12 solution{};
    solution.set_input("./data/12/example7.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 1);
  }

  SUBCASE("Part 1, example 8") {
    Day12 solution{};
    solution.set_input("./data/12/example8.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 10);
  }

  SUBCASE("Part 1, example 9") {
    Day12 solution{};
    solution.set_input("./data/12/example9.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3);
  }

  SUBCASE("Part 1, example 10") {
    Day12 solution{};
    solution.set_input("./data/12/example10.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2);
  }

  SUBCASE("Part 1, example 11") {
    Day12 solution{};
    solution.set_input("./data/12/example11.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2);
  }

  SUBCASE("Part 1, example 12") {
    Day12 solution{};
    solution.set_input("./data/12/example12.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3);
  }

  SUBCASE("Part 1, example 13") {
    Day12 solution{};
    solution.set_input("./data/12/example13.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 2);
  }

  SUBCASE("Part 1, example 14") {
    Day12 solution{};
    solution.set_input("./data/12/example14.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3);
  }

  SUBCASE("Part 1, example 15") {
    Day12 solution{};
    solution.set_input("./data/12/example15.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 4);
  }

  SUBCASE("Part 2, example 1") {
    Day12 solution{};
    solution.set_input("./data/12/example1.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 16384);
  }

  SUBCASE("Part 2, example 2") {
    Day12 solution{};
    solution.set_input("./data/12/example2.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 1);
  }

  SUBCASE("Part 2, example 3") {
    Day12 solution{};
    solution.set_input("./data/12/example3.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 506250);
  }

  SUBCASE("Part 2, example 4") {
    Day12 solution{};
    solution.set_input("./data/12/example4.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 1);
  }

  SUBCASE("Part 2, example 6") {
    Day12 solution{};
    solution.set_input("./data/12/example6.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 525152);
  }

  SUBCASE("Part 2, example 15") {
    Day12 solution{};
    solution.set_input("./data/12/example15.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 5184);
  }

  SUBCASE("Real data") {
    Day12 solution{};
    solution.set_input("./data/12/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 7653);
    REQUIRE_GT(solution.part2(), 2121046676);
    REQUIRE_GT(solution.part2(), 75135490708);
  }
}