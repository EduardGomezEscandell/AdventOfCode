#include "day01.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 1") {
    Day01 solution{};

    SUBCASE("Part 1") {
        solution.set_input("./data/01/example1.txt");
        solution.load();
        REQUIRE_EQ(solution.day(), 1);
    }

    SUBCASE("Part 2") {
        solution.set_input("./data/01/example2.txt");
        solution.load();
        REQUIRE_EQ(solution.part2(), 281);

         solution.set_input("./data/01/example3.txt");
         solution.load();
        REQUIRE_EQ(solution.part2(), 33);
    }

    SUBCASE("Real data") {
        solution.set_input("./data/01/input.txt");
        solution.load();
        REQUIRE_EQ(solution.part1(), 54561);
        REQUIRE_EQ(solution.part2(), 54076);
    }
}