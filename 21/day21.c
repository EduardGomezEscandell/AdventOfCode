#include "day21.h"

#include "common/file_utils.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>

void ReadInitialPositions(bool is_test, int * player_1, int * player_2)
{
    FILE * file = GetFile(is_test, 21);

    size_t len = 0;
    char * line = NULL;

    if(getline(&line, &len, file) == -1) {
        fprintf(stderr, "File missing player 1 starting position");
        exit(EXIT_FAILURE);
    }


    *player_1 = atoi(line + 28);

    if(getline(&line, &len, file) == -1) {
        fprintf(stderr, "File missing player 1 starting position");
        exit(EXIT_FAILURE);
    }

    *player_2 = atoi(line + 28);

    free(line);
    fclose(file);
}

int RollDeterministic(int * die)
{
    int acc = 0;
    for(size_t i=0; i<3; ++i)
    {
        *die = (*die) % 100 + 1; // 100 is the number of faces on the die
        acc += *die;
    }
    return acc;
}

int MovePawn(int position, int roll)
{
    return (position  + roll - 1) % BOARD_SIZE + 1;
}

solution_t SolvePart1(const bool is_test)
{
    int player_1, player_2;

    ReadInitialPositions(is_test, &player_1, &player_2);

    unsigned long n_rolls = 0;
    int die = 0;

    solution_t score_1 = 0;
    solution_t score_2 = 0;

    const solution_t goal = 1000;

    while(true)
    {
        player_1 = MovePawn(player_1, RollDeterministic(&die));
        score_1 += player_1;
        n_rolls += 3;

        if(score_1 >= goal) return score_2 * n_rolls;

        player_2 = MovePawn(player_2, RollDeterministic(&die));
        score_2 += player_2;
        n_rolls += 3;

        if(score_2 >= goal) return score_1 * n_rolls;
    }
}

bool * IsCached(Cache * cache, int player_1, int player_2, int score_1, int score_2)
{
    return & cache->known[player_1-1][player_2-1][score_1][score_2];
}

WinRates * GetCache(Cache * cache, int player_1, int player_2, int score_1, int score_2)
{
    return & (cache->winrates[player_1-1][player_2-1][score_1][score_2]);
}

Cache NewCache()
{
    Cache cache;
    for(size_t i=0; i<BOARD_SIZE; ++i)
        for(size_t j=0; j<BOARD_SIZE; ++j)
            for(size_t m=0; m<PART_2_WIN_SCORE; ++m)
                for(size_t n=0; n<PART_2_WIN_SCORE; ++n)
                    cache.known[i][j][m][n] = false;
    return cache;
}

#define N_COMBINATIONS 7

void BinomalPair(size_t i, int * roll, int * weight)
{
    if(i >= N_COMBINATIONS)
    {
        *roll = 0;
        *weight = 0;
        return;
    }

    static const int rolls[7] =
    {
        3, 4, 5, 6, 7, 8, 9
    };

    static const int weights[7] =
    {
        1, 3, 6, 7, 6, 3, 1
    };

    *roll = rolls[i];
    *weight = weights[i];
}

WinRates * NewUniverse(Cache * cache, int player_1, int player_2, int score_1, int score_2)
{
    bool * is_cached = IsCached(cache, player_1, player_2, score_1, score_2);
    WinRates * result = GetCache(cache, player_1, player_2, score_1, score_2);

    if(*is_cached) return result;

    result->player_1 = 0;
    result->player_2 = 0;

    int roll = 1;
    int weight_1 = 1;

    for(size_t i=0; weight_1 != 0; ++i)
    {
        BinomalPair(i, &roll, &weight_1);

        int p1 = MovePawn(player_1, roll);
        int s1 = score_1 + p1;

        if(s1 >= PART_2_WIN_SCORE)
        {
            result->player_1 += weight_1;
            continue;
        }

        int weight_2 = 1;
        for(size_t j=0; weight_2 != 0; ++j)
        {
            BinomalPair(j, &roll, &weight_2);

            int p2 = MovePawn(player_2, roll);
            int s2 = score_2 + p2;

            if(s2 >= PART_2_WIN_SCORE)
            {
                result->player_2 += weight_1 * weight_2;
                continue;
            }

            WinRates * continuations = NewUniverse(cache, p1, p2, s1, s2);

            result->player_1 += weight_1 * weight_2 * continuations->player_1;
            result->player_2 += weight_1 * weight_2 * continuations->player_2;
        }
    }

    *is_cached = true;
    return result;
}

solution_t SolvePart2(const bool is_test)
{
    int player_1, player_2;

    ReadInitialPositions(is_test, &player_1, &player_2);

    Cache cache = NewCache();
    WinRates * wr = NewUniverse(&cache, player_1, player_2, 0, 0);

    return MAX(wr->player_1, wr->player_2);
}


DEFINE_TEST(1, 739785)
DEFINE_TEST(2, 444356092776315)
