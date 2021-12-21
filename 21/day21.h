#ifndef DAY_21_H
#define DAY_21_H

#include "common/testing.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#define BOARD_SIZE 10
#define PART_2_WIN_SCORE 21

typedef uintmax_t solution_t; // Just in case we have to switch to long

typedef struct {
    solution_t player_1;
    solution_t player_2;
} WinRates;

// This could be optimized: the game is symmetrical
typedef struct {
    WinRates winrates[BOARD_SIZE][BOARD_SIZE][PART_2_WIN_SCORE][PART_2_WIN_SCORE];
    bool     known   [BOARD_SIZE][BOARD_SIZE][PART_2_WIN_SCORE][PART_2_WIN_SCORE];
} Cache;


void ReadInitialPositions(bool is_test, int * player_1, int * player_2);

int RollDeterministic(int * die);
int MovePawn(int position, int roll);

bool * IsCached(Cache * cache, int player_1, int player_2, int score_1, int score_2);
WinRates * GetCache(Cache * cache, int player_1, int player_2, int score_1, int score_2);

Cache NewCache();

void BinomalPair(size_t i, int * roll, int * weight);
WinRates * NewUniverse(Cache * cache, int player_1, int player_2, int score_1, int score_2);

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
