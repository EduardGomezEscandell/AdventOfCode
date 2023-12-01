#include "day01.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 1") {
    Day01 solution{};

    SUBCASE("Part 1") {
        solution.load("./data/01/example1.txt");
        REQUIRE_EQ(solution.day(), 1);
    }

    SUBCASE("Part 2") {
        solution.load("./data/01/example2.txt");
        REQUIRE_EQ(solution.part2(), 281);

         solution.load("./data/01/example3.txt");
        REQUIRE_EQ(solution.part2(), 33);
    }

    SUBCASE("Real data") {
        solution.load("./data/01/input.txt");
        REQUIRE_EQ(solution.part1(), 54561);
        REQUIRE_EQ(solution.part2(), 54076);
    }
}