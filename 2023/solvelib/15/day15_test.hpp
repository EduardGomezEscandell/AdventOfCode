#include "day15.hpp"
#include <doctest/doctest.h>

#include <string_view>

TEST_CASE("Day 15") {

  SUBCASE("Hashing") {
    using namespace std::string_view_literals;

    // Compile-time testing!
    static_assert(Day15::hash256(""sv) == 0);
    static_assert(Day15::hash256("H"sv) == 200);
    static_assert(Day15::hash256("HA"sv) == 153);
    static_assert(Day15::hash256("HAS"sv) == 172);
    static_assert(Day15::hash256("HASH"sv) == 52);

    static_assert(Day15::hash256("rn=1"sv) == 30);
    static_assert(Day15::hash256("cm-"sv) == 253);
    static_assert(Day15::hash256("qp=3"sv) == 97);
    static_assert(Day15::hash256("cm=2"sv) == 47);
    static_assert(Day15::hash256("qp-"sv) == 14);
    static_assert(Day15::hash256("pc=4"sv) == 180);
    static_assert(Day15::hash256("ot=9"sv) == 9);
    static_assert(Day15::hash256("ab=5"sv) == 197);
    static_assert(Day15::hash256("pc-"sv) == 48);
    static_assert(Day15::hash256("pc=6"sv) == 214);
    static_assert(Day15::hash256("ot=7"sv) == 231);
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
    REQUIRE_EQ(solution.part2(), 145);
  }

  SUBCASE("Real data") {
    Day15 solution{};
    solution.set_input("./data/15/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 516804);
    REQUIRE_EQ(solution.part2(), 231844);
  }
}