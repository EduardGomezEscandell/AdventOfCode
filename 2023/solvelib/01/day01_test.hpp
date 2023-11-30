#include "day01.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 1") {
    Day01 solution{};

    REQUIRE_EQ(solution.day(), 1);
    REQUIRE_EQ(solution.part1(), 1);
    REQUIRE_EQ(solution.part2(), 2);
}