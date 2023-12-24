#include "day22.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 22") {

  SUBCASE("Part 1, example") {
    Day22 solution{};
    solution.set_input("./data/22/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 5);
  }

  SUBCASE("Part 1, test") {
    Day22 solution{};
    solution.set_input("./data/22/test.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 9);
  }

  SUBCASE("Part 1, test 2") {
    Day22 solution{};
    solution.set_input("./data/22/test2.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3);
  }

  SUBCASE("Part 1, test 3") {
    Day22 solution{};
    solution.set_input("./data/22/test3.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3);
  }

  SUBCASE("Part 2, example") {
    Day22 solution{};
    solution.set_input("./data/22/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 7);
  }

  SUBCASE("Part 2, test 3") {
    Day22 solution{};
    solution.set_input("./data/22/test3.txt");
    solution.load();
    REQUIRE_EQ(solution.part2(), 5);
  }

  SUBCASE("Real data") {
    Day22 solution{};
    solution.set_input("./data/22/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 492);
    REQUIRE_EQ(solution.part2(), 86556);
  }
}