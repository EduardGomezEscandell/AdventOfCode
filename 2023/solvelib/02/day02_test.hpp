#include "day02.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 2") {
    Day02 solution{};

    SUBCASE("Part 2") {
        solution.load("./data/02/example1.txt");
        // REQUIRE_EQ(solution.day(), 1);
    }
}