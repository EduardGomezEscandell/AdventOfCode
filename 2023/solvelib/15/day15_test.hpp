#include "day15.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 15") {

  SUBCASE("Hashing") {
    REQUIRE_EQ(Day15::hash256(""), 0);
    REQUIRE_EQ(Day15::hash256("H"), 200);
    REQUIRE_EQ(Day15::hash256("HA"), 153);
    REQUIRE_EQ(Day15::hash256("HAS"), 172);
    REQUIRE_EQ(Day15::hash256("HASH"), 52);

    REQUIRE_EQ(Day15::hash256("rn=1"), 30);
    REQUIRE_EQ(Day15::hash256("cm-"), 253);
    REQUIRE_EQ(Day15::hash256("qp=3"), 97);
    REQUIRE_EQ(Day15::hash256("cm=2"), 47);
    REQUIRE_EQ(Day15::hash256("qp-"), 14);
    REQUIRE_EQ(Day15::hash256("pc=4"), 180);
    REQUIRE_EQ(Day15::hash256("ot=9"), 9);
    REQUIRE_EQ(Day15::hash256("ab=5"), 197);
    REQUIRE_EQ(Day15::hash256("pc-"), 48);
    REQUIRE_EQ(Day15::hash256("pc=6"), 214);
    REQUIRE_EQ(Day15::hash256("ot=7"), 231);
  }

  SUBCASE("Part 1, example") {
    Day15 solution{};
    solution.set_input("./data/15/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 1320);
  }

  SUBCASE("Part 2, example") {
    Day15 solution{};
    solution.set_input("./data/15/example.txt");
    solution.load();
    REQUIRE_THROWS(solution.part2());
  }

  SUBCASE("Real data") {
    Day15 solution{};
    solution.set_input("./data/15/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 516804);
    REQUIRE_THROWS(solution.part2());
  }
}