#include "day09.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 09") {

  SUBCASE("Part 1, test 1") {
    Day09 solution{};
    solution.set_input("./data/09/test1.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 9);
  }

  SUBCASE("Part 1, test 2") {
    Day09 solution{};
    solution.set_input("./data/09/test2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 119395);
  }

  SUBCASE("Part 1, example") {
    Day09 solution{};
    solution.set_input("./data/09/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 114);
  }

  SUBCASE("Part 2, example") {
    Day09 solution{};
    solution.set_input("./data/09/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day09 solution{};
    solution.set_input("./data/09/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 1806615041);
    REQUIRE_THROWS(solution.part2());
  }
}