#include "day02.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 2") {
    Day02 solution{};

    SUBCASE("Part 1") {
        solution.set_input("./data/02/example1.txt");
        solution.load();
        CHECK_THROWS(solution.part1());
    }

    SUBCASE("Part 2") {
        solution.set_input("./data/02/example1.txt");
        solution.load();
        CHECK_THROWS(solution.part2());
    }
}