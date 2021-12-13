#ifndef DAY_05_H
#define DAY_05_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common/testing.h"

#define BOARD_SIZE_X 1000
#define BOARD_SIZE_Y 1000

typedef struct
{
	int data[BOARD_SIZE_X][BOARD_SIZE_Y];
} Board;

typedef struct
{
	size_t x1;
	size_t y1;
	size_t x2;
	size_t y2;
} Vent;

// Board
Board EmptyBoard();
void PrintBoard(const Board * const b);
unsigned int CountOverlaps(const Board * const b);

// Vents
void AddVentToBoard(
	Board * const board,
	const Vent * const v,
	const bool consider_diagonals);
Vent LineToVent(char * const line);
void ReadVents(FILE * file, Board * board, const bool consider_diagonals);

// Solving
int SolvePart1(const bool is_test);
int SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
