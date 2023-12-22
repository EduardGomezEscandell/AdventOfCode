#include "day21.hpp"
#include <doctest/doctest.h>

TEST_CASE("Day 21") {

  SUBCASE("Part 1, example") {
    Day21 solution{};
    solution.set_input("./data/21/example.txt");
    solution.load();
    REQUIRE_EQ(solution.part1_generalized(1), 2);
    REQUIRE_EQ(solution.part1_generalized(2), 4);
    REQUIRE_EQ(solution.part1_generalized(3), 6);
    REQUIRE_EQ(solution.part1_generalized(6), 16);
  }

  SUBCASE("Part 2, test") {
    Day21 solution{};
    solution.set_input("./data/21/test.txt");
    solution.load();
    REQUIRE_EQ(solution.part2_generalized(4), 25);
    //
    /*
    ···|·4·|···
    ···|434|···
    ··4|323|4··
    ---+---+---
    ·43|212|34·
    432|1S1|234
    ·43|212|34·
    ---+---+---
    ··4|323|4··
    ···|434|···
    ···|·4·|···

    Colours:
    OPO
    PBP
    OPO

    Counts:
              4    2    0   TOTAL
    -----------------------------------
    blue:     0    4    1    5
    orange:   4    0    0    4
    yellow:   0    0    0    0
    purple:  12    4    0   16
    TOTAL    16    8    1   25
    */

    REQUIRE_EQ(solution.part2_generalized(7), 64);
    /*
    ···|···|·7·|···|···
    ···|···|767|···|···
    ···|··7|656|7··|···
    ---+---+---+---+---
    ···|·76|545|67·|···
    ···|765|434|567|···
    ··7|654|323|456|7··
    ---+---+---+---+---
    ·76|543|212|345|67·
    765|432|1S1|234|567
    ·76|543|212|345|67·
    ---+---+---+---+---
    ··7|654|323|456|7··
    ···|765|434|567|···
    ···|·76|545|67·|···
    ---+---+---+---+---
    ···|··7|656|7··|···
    ···|···|767|···|···
    ···|···|·7·|···|···


    Colours:
    ·OPO·
    OYBYO
    PBBBP
    OYBYO
    ·OPO·

    Counts:

              7    5    3    1   TOTAL
    -----------------------------------
    blue:     0    8   12    4    24
    orange:   8    0    0    0     8
    yellow:   8    8    0    0    16
    purple:  12    4    0    0    16
    TOTAL    28   20   12    4    64
    */
  }

  SUBCASE("Real data") {
    Day21 solution{};
    solution.set_input("./data/21/input.txt");
    solution.load();
    REQUIRE_EQ(solution.part1(), 3733);
    REQUIRE_EQ(solution.part2(), 617729401414635);
  }
}