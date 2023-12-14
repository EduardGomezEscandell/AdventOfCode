#include "day13.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 13") {
  Day13 solution{};

  SUBCASE("Part 1, test 1") {
    solution.set_input("./data/13/test1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 5);
  }

  SUBCASE("Part 1, test 2") {
    solution.set_input("./data/13/test2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 400);
  }

  SUBCASE("Part 1, example") {
    solution.set_input("./data/13/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 405);
  }

  SUBCASE("Part 2, test 1") {
    solution.set_input("./data/13/test1.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 300);
  }

  SUBCASE("Part 2, test 2") {
    solution.set_input("./data/13/test2.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 100);
  }

  SUBCASE("Part 2, example") {
    solution.set_input("./data/13/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 400);
  }

  SUBCASE("Real data") {
    solution.set_input("./data/13/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 39939);
    REQUIRE_EQ(solution.part2(), 32069);
  }
}