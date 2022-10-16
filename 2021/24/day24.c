#include "day24.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  From input we know:
 *
 *   Digit #       MIN     MAX     EXACT VALUE
 *      0           4       9
 *      1           1       6
 *      2           8       9
 *      3           -       -       d2-7
 *      4           1       9
 *      5           7       9
 *      6           6       9
 *      7           -       -       d6-5
 *      8           1       2
 *      9           -       -       d8+7
 *     10           -       -       d5-6
 *     11           -       -       d4
 *     12           -       -       d1+3
 *     13           -       -       d0-3
 * 
 * These values will be diferent for each input.
 */

solution_t AssembleNumber(enum Target target)
{

    int min_values[MONAD_DIGITS] = {4,1,8,0,1,7,6,0,1,0,0,0,0,0};
    int max_values[MONAD_DIGITS] = {9,6,9,0,9,9,9,0,2,0,0,0,0,0};

    int * digits = (target == MAX) ? max_values : min_values;
    
    solution_t d = 0;
    for(size_t i=0; i<MONAD_DIGITS; ++i)
    {
        d*=10;
        switch (i) {
            case  3: d += digits[2] - 7; break;
            case  7: d += digits[6] - 5; break;
            case  9: d += digits[8] + 7; break;
            case 10: d += digits[5] - 6; break;
            case 11: d += digits[4] + 0; break;
            case 12: d += digits[1] + 3; break;
            case 13: d += digits[0] - 3; break;
            default: d += digits[i];
        }
    }

    return d;
}


solution_t SolvePart1(const bool is_test)
{
    if(is_test) return AssembleNumber(MAX); // Here to suppress warning
    
    return AssembleNumber(MAX);
}


solution_t SolvePart2(const bool is_test)
{
    if(is_test) return AssembleNumber(MIN); // Here to suppress warning

    return AssembleNumber(MIN);
}

DEFINE_TEST(1, 96929994293996)
DEFINE_TEST(2, 41811761181141)
